#include "adapter/mqtt/GMqttAdapter.hpp"
#include <qassert.h>

#include <QMetaObject>
#include <QPointer>
#include <QScopedValueRollback>
#include <QThread>
#include <string>

#include "utils/TLog.hpp"

#define T_LOG_TAG "[MQTT Adapter] "

using namespace std;

namespace gentau {
GMqttAdapter::GMqttAdapter(QObject* parent) : QObject(parent)
{
	tLogDebug(
		"Adapter created, parent: {}", parent ? parent->objectName().toStdString() : "<nullptr>"
	);
}

GMqttAdapter::~GMqttAdapter()
{
	shuttingDown.store(true);
	generation.fetch_add(1);

	topicRegister.clear();

	auto oldSnapshot = snapshot.exchange(nullptr);
	oldSnapshot.reset();

	tLogDebug("Adapter destroyed");
}

auto GMqttAdapter::bind(const QString& clientId, const QString& serverURI) -> BindResult
{
	Q_ASSERT(QThread::currentThread() == thread());  // No cross-thread calls
	Q_ASSERT(!bindingInProgress);                    // No reentrant calls

	QScopedValueRollback<bool> rollback(bindingInProgress, true);

	if (shuttingDown.load()) {
		return BindResult{ BindStatus::Failed, std::nullopt, "Adapter is shutting down" };
	}

	string idStdStr  = clientId.toStdString();
	string uriStdStr = serverURI.toStdString();

	const auto             oldSnapshot = snapshot.load();
	TMqttClient::SharedPtr oldClient   = oldSnapshot ? oldSnapshot->client : nullptr;

	quint64 curGen = oldSnapshot ? oldSnapshot->generation : 0;

	if (oldClient && oldClient->getClientId() == idStdStr &&
		oldClient->getServerURI() == uriStdStr) {
		tLogWarn("Binding unchanged, incomming clientId and serverURI same as current");
		return BindResult{ BindStatus::Unchanged, curGen, "" };
	}

	Q_EMIT bindingStarted(clientId, serverURI, curGen);

	tLogInfo(
		"Rebinding requested, clientId: '{}', serverURI: '{}', current generation: {}",
		idStdStr,
		uriStdStr,
		curGen
	);

	TMqttClient::SharedPtr newClient = nullptr;
	try {
		newClient = TMqttClient::create(idStdStr, uriStdStr);
	} catch (const std::exception& e) {
		Q_EMIT bindingFailed(clientId, serverURI, e.what());
		tLogError(
			"Failed to create new MQTT client with clientId: '{}', serverURI: '{}', cause: {}",
			idStdStr,
			uriStdStr,
			e.what()
		);

		return BindResult{ BindStatus::Failed, std::nullopt, e.what() };
	}

	const quint64 newGen = ++generation;

	// --------------------  Qt Signal installation --------------------
	newClient->onConnected += [this, newGen]() {
		QMetaObject::invokeMethod(
			this,
			[this, newGen]() {
				const auto currentSnap = snapshot.load();
				if (!currentSnap || currentSnap->generation != newGen) {
					tLogDebug(
						"signal 'connected' received for outdated generation: {}, expected: {}",
						newGen,
						currentSnap ? currentSnap->generation : 0
					);
					return;
				}

				Q_EMIT connected();
			},
			Qt::QueuedConnection
		);
	};

	newClient->onConnectionLost += [this, newGen](const std::string& cause) {
		QMetaObject::invokeMethod(
			this,
			[this, newGen, cause]() {
				const auto currentSnap = snapshot.load();
				if (!currentSnap || currentSnap->generation != newGen) {
					tLogDebug(
						"signal 'connectionLost' received for outdated generation: {}, expected: "
						"{}",
						newGen,
						currentSnap ? currentSnap->generation : 0
					);
					return;
				}

				Q_EMIT connectionLost(QString::fromStdString(cause));
			},
			Qt::QueuedConnection
		);
	};

	newClient->onConnectionFailed += [this, newGen](const std::string& cause) {
		QMetaObject::invokeMethod(
			this,
			[this, newGen, cause]() {
				const auto currentSnap = snapshot.load();
				if (!currentSnap || currentSnap->generation != newGen) {
					tLogDebug(
						"signal 'connectionFailed' received for outdated generation: {}, expected: "
						"{}",
						newGen,
						currentSnap ? currentSnap->generation : 0
					);
					return;
				}

				Q_EMIT connectionFailed(QString::fromStdString(cause));
			},
			Qt::QueuedConnection
		);
	};

	newClient->onSubSyncFailed += [this,
								   newGen](const std::unordered_set<std::string>& failedTopics) {
		QMetaObject::invokeMethod(
			this,
			[this, newGen, failedTopics]() {
				const auto currentSnap = snapshot.load();
				if (!currentSnap || currentSnap->generation != newGen) {
					tLogDebug(
						"signal 'subSyncFailed' received for outdated generation: {}, expected: "
						"{}",
						newGen,
						currentSnap ? currentSnap->generation : 0
					);
					return;
				}

				QStringList failedTopicsList;
				for (const auto& topic : failedTopics) {
					failedTopicsList.append(QString::fromStdString(topic));
				}
				Q_EMIT subSyncFailed(failedTopicsList);
			},
			Qt::QueuedConnection
		);
	};
	// --------------------  Qt Signal installation --------------------

	auto newSnapshot = std::make_shared<const BindingSnapshot>(newGen, newClient);

	topicRegister.clear();
	tLogDebug("New generation '{}' assigned, old topic register cleared", newGen);

	snapshot.store(newSnapshot);

	Q_EMIT bindingChanged(clientId, serverURI, newGen);

	newSnapshot->client->connect();

	tLogInfo(
		"Binding changed, clientId: '{}', serverURI: '{}', new generation: {}",
		idStdStr,
		uriStdStr,
		newGen
	);

	return BindResult{ BindStatus::Changed, newGen, "" };
}

auto GMqttAdapter::registerTopic(QObject* context, const std::string& topic, TopicHandler handler)
	-> RegisterResult
{
	Q_ASSERT(QThread::currentThread() == thread());  // No cross-thread calls
	Q_ASSERT(context);                               // Context must not be null

	// Q_ASSERT(context->thread() == thread());  // Context must be in the same thread as the adapter

	if (shuttingDown.load()) {
		Q_EMIT registerRejected(
			QString::fromStdString(topic),
			RegisterRejectReason::AdapterRejected,
			QStringLiteral("Adapter is shutting down")
		);

		return RegisterResult{ std::nullopt,
							   RegisterRejectReason::AdapterRejected,
							   QStringLiteral("Adapter is shutting down") };
	}

	if (!context) {
		const QString cause = QStringLiteral("Context is null");

		Q_EMIT registerRejected(
			QString::fromStdString(topic), RegisterRejectReason::AdapterRejected, cause
		);

		tLogWarn("Failed to register topic '{}', cause: Context is null", topic);

		return RegisterResult{ std::nullopt, RegisterRejectReason::AdapterRejected, cause };
	}

	if (topic.empty()) {
		const QString cause = QStringLiteral("Topic is empty");

		Q_EMIT registerRejected(
			QString::fromStdString(topic), RegisterRejectReason::AdapterRejected, cause
		);

		tLogWarn("Failed to register topic '{}', cause: Topic is empty", topic);

		return RegisterResult{ std::nullopt, RegisterRejectReason::AdapterRejected, cause };
	}

	if (!handler) {
		const QString cause = QStringLiteral("Topic handler is null");

		Q_EMIT registerRejected(
			QString::fromStdString(topic), RegisterRejectReason::AdapterRejected, cause
		);

		tLogWarn("Failed to register topic '{}', cause: Topic handler is null", topic);

		return RegisterResult{ std::nullopt, RegisterRejectReason::AdapterRejected, cause };
	}

	const auto providedSnap = snapshot.load();

	if (!providedSnap || !providedSnap->isValid()) {
		const QString cause = QStringLiteral("No valid binding");

		Q_EMIT registerRejected(
			QString::fromStdString(topic), RegisterRejectReason::NoBinding, cause
		);

		tLogWarn("Failed to register topic '{}', cause: No valid binding", topic);

		return RegisterResult{ std::nullopt, RegisterRejectReason::NoBinding, cause };
	}

	const auto providedGen = providedSnap->generation;
	auto       handlerPtr  = std::make_shared<TopicHandler>(std::move(handler));

	auto [topicRegIter, inserted] =
		topicRegister.try_emplace(topic, std::make_unique<GMqttChannel>(this));

	if (inserted) {
		auto tConn = providedSnap->client->registerTopic(
			topic, [this, topic, providedGen](const string& payload) {
				QByteArray data(payload.data(), static_cast<qsizetype>(payload.size()));

				QMetaObject::invokeMethod(
					this,
					[this, topic, providedGen, data = std::move(data)]() {
						const auto currentSnap = snapshot.load();
						if (!currentSnap || currentSnap->generation != providedGen) { return; }

						auto iter = topicRegister.find(topic);
						if (iter == topicRegister.end()) { return; }

						iter->second->received(providedGen, data);
					},
					Qt::QueuedConnection
				);
			}
		);

		topicRegIter->second->setConnection(std::move(tConn));
	}

	auto qConnHndl = QObject::connect(
		topicRegIter->second.get(),
		&GMqttChannel::received,
		context,
		[this, handlerPtr](quint64 gen, const QByteArray& payload) {
			const auto currentSnap = snapshot.load();
			if (!currentSnap || currentSnap->generation != gen) { return; }

			(*handlerPtr)(payload);
		},
		Qt::AutoConnection
	);

	return RegisterResult{ qConnHndl, std::nullopt, "" };
}

auto GMqttAdapter::publish(
	quint64 reqGen, const std::string& topic, const QByteArray& payload, TMqttClient::QoS qos
) -> PublishResult
{
	if (shuttingDown.load()) {
		const QString cause = QStringLiteral("Adapter is shutting down");

		Q_EMIT publishRejected(
			QString::fromStdString(topic), PublishRejectReason::AdapterRejected, cause, reqGen
		);

		return PublishResult{ PublishRejectReason::AdapterRejected, cause };
	}

	const auto currentSnap = snapshot.load();

	if (!currentSnap || !currentSnap->isValid()) {
		const QString cause = QStringLiteral("No valid binding");

		Q_EMIT publishRejected(
			QString::fromStdString(topic), PublishRejectReason::NoBinding, cause, reqGen
		);

		tLogWarn("Failed to publish to topic '{}', cause: No valid binding", topic);

		return PublishResult{ PublishRejectReason::NoBinding, cause };
	}

	if (currentSnap->generation != reqGen) {
		const QString cause = QStringLiteral("Stale generation");

		Q_EMIT publishRejected(
			QString::fromStdString(topic), PublishRejectReason::StaleGeneration, cause, reqGen
		);

		tLogWarn(
			"Failed to publish to topic '{}', cause: Stale generation, provided: {}, current: {}",
			topic,
			reqGen,
			currentSnap->generation
		);

		return PublishResult{ PublishRejectReason::StaleGeneration, cause };
	}

	try {
		currentSnap->client->publish(topic, payload.toStdString(), qos);
	} catch (const std::exception& e) {
		const QString cause = QString::fromStdString(e.what());

		Q_EMIT publishRejected(
			QString::fromStdString(topic), PublishRejectReason::ClientRejected, cause, reqGen
		);

		tLogWarn(
			"Failed to publish to topic '{}', cause: Client rejected, error: {}", topic, e.what()
		);

		return PublishResult{ PublishRejectReason::ClientRejected, cause };
	}

	return PublishResult{ std::nullopt, "" };
}
}  // namespace gentau