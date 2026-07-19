#pragma once

#include "comm/TMqttClient.hpp"

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QStringList>

#include <atomic>
#include <functional>
#include <optional>
#include <string>

namespace gentau {
class GMqttAdapter : public QObject
{
	Q_OBJECT

  public:
	using TConn        = gentau::Connection;
	using TopicHandler = std::function<void(const QByteArray& payload)>;

	enum class BindStatus : quint32
	{
		Changed = 0,
		Unchanged,
		Failed
	};

	struct BindResult
	{
		BindStatus             status{ BindStatus::Failed };
		std::optional<quint64> newGen;
		QString                failedReason;

		[[nodiscard]] bool succeeded() const noexcept { return status != BindStatus::Failed; }

		[[nodiscard]] bool changed() const noexcept { return status == BindStatus::Changed; }
	};

	enum class RegisterRejectReason : quint32
	{
		NoBinding = 0,
		// ClientRejected,
		AdapterRejected,
		Unknown
	};

	struct RegisterResult
	{
		std::optional<TConn>                conn;
		std::optional<RegisterRejectReason> rejectReason;
		QString                             cause;

		[[nodiscard]] bool succeeded() const noexcept { return conn.has_value(); }
	};

	enum class PublishRejectReason : quint32
	{
		StaleGeneration = 0,
		NoBinding,
		ClientRejected,
		AdapterRejected,
		Unknown
	};

	struct PublishResult
	{
		std::optional<PublishRejectReason> rejectReason;
		QString                            cause;

		[[nodiscard]] bool succeeded() const noexcept { return !rejectReason.has_value(); }
	};

  private:
	struct BindingSnapshot
	{
		quint64                generation{ 0 };
		TMqttClient::SharedPtr client{ nullptr };

		[[nodiscard]] bool isValid() const noexcept { return client != nullptr && generation > 0; }
	};

	using AtomicBindingSnapshot = std::atomic<std::shared_ptr<const BindingSnapshot>>;

  Q_SIGNALS:
	void connected();
	void connectionLost(const QString& cause);
	void connectionFailed(const QString& cause);
	void subSyncFailed(const QStringList& failedTopics);

	void bindingStarted(const QString& reqId, const QString& reqUri, quint64 curGen);
	void bindingChanged(const QString& newId, const QString& newUri, quint64 newGen);
	void bindingFailed(const QString& reqId, const QString& reqUri, const QString& cause);

	void registerRejected(
		const QString& topic, RegisterRejectReason rejectReason, const QString& cause
	);

	void publishRejected(
		const QString&      topic,
		PublishRejectReason rejectReason,
		const QString&      cause,
		quint64             providedGen
	);

  public:
	BindResult bind(const QString& clientId, const QString& serverURI);

	RegisterResult registerTopic(const std::string& topic, TopicHandler handler);

	PublishResult publish(
		quint64            reqGen,
		const std::string& topic,
		const QByteArray&  payload,
		TMqttClient::QoS   qos = TMqttClient::QoS::AT_LEAST_ONCE
	);

  private:
	std::atomic<bool> shuttingDown{ false };

	bool bindingInProgress{ false };

	std::atomic<quint64> generation{ 0 };

	AtomicBindingSnapshot snapshot{ nullptr };

  public:
	explicit GMqttAdapter(QObject* parent = nullptr);
	~GMqttAdapter() override;
};
}  // namespace gentau