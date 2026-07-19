#pragma once

#include "comm/TMqttClient.hpp"

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QStringList>

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <version>

#if defined(__cpp_lib_atomic_shared_ptr) && __cpp_lib_atomic_shared_ptr >= 201711L
#define G_MQTT_USE_ATOMIC_SHARED_PTR
#endif

namespace gentau {
class GMqttChannel final : public QObject
{
	Q_OBJECT

  private:
	ScopedConnection conn;

  Q_SIGNALS:
	void received(quint64 gen, const QByteArray& payload);

  public:
	void setConnection(ScopedConnection&& newConn) { conn = std::move(newConn); }

	explicit GMqttChannel(QObject* parent = nullptr) : QObject(parent) {};

	~GMqttChannel() override = default;
};

class GMqttAdapter : public QObject
{
	Q_OBJECT

  public:
	using TConn         = gentau::Connection;
	using TopicHandler  = std::function<void(const QByteArray& payload)>;
	using TopicRegister = std::unordered_map<std::string, std::unique_ptr<GMqttChannel> >;

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
		AdapterRejected,
		Unknown
	};

	struct RegisterResult
	{
		std::optional<QMetaObject::Connection> conn;
		std::optional<RegisterRejectReason>    rejectReason;
		QString                                cause;

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

#ifndef G_MQTT_USE_ATOMIC_SHARED_PTR
	struct AtomicBindingSnapshot
	{
		using BindingSnapshotPtr = std::shared_ptr<const BindingSnapshot>;

		BindingSnapshotPtr ptr;

		mutable std::mutex mtx;

		[[nodiscard]] std::shared_ptr<const BindingSnapshot> load() const
		{
			std::lock_guard lock(mtx);
			return ptr;
		}

		void store(std::shared_ptr<const BindingSnapshot> newPtr)
		{
			(void)exchange(std::move(newPtr));
		}

		[[nodiscard]] std::shared_ptr<const BindingSnapshot> exchange(
			std::shared_ptr<const BindingSnapshot> newPtr
		)
		{
			std::lock_guard lock(mtx);
			ptr.swap(newPtr);
			return newPtr;
		}
	};
#else
	using AtomicBindingSnapshot = std::atomic<std::shared_ptr<const BindingSnapshot> >;
#endif

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

	RegisterResult registerTopic(const std::string& topic, TopicHandler handler, QObject* context);

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

	TopicRegister topicRegister;

  public:
	explicit GMqttAdapter(QObject* parent = nullptr);
	~GMqttAdapter() override;
};
}  // namespace gentau