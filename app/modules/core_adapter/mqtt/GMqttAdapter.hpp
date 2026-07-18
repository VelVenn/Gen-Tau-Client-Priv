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
		std::optional<QString> newGen;
		QString                failedReason;

		[[nodiscard]] bool succeeded() const noexcept { return status != BindStatus::Failed; }

		[[nodiscard]] bool changed() const noexcept { return status == BindStatus::Changed; }
	};

	enum class PublishStatus : quint32
	{
		Accepted = 0,
		StaleGeneration,
        Rejected
	};

	struct PublishResult
	{
		PublishStatus status{ PublishStatus::Rejected };
		QString       rejectedReason;

		[[nodiscard]] bool succeeded() const noexcept { return status == PublishStatus::Accepted; }
	};

  Q_SIGNALS:
	void connected();
	void connectionLost(const QString& cause);
	void connectionFailed(const QString& cause);
	void subSyncFailed(const QStringList& failedTopics);

	void rebindStarted(const QString& reqId, const QString& reqUri, quint64 curGen);
	void bindingChanged(const QString& newId, const QString& newUri, quint64 newGen);
	void bindingFailed(const QString& reqId, const QString& reqUri, const QString& cause);

    void publishRejected(const QString& topic, const QString& cause, quint64 providedGen);

  public:
	BindResult bind(const QString& clientId, const QString& serverURI);

	std::optional<TConn> registerTopic(
		quint64 providedGen, const std::string& topic, TopicHandler handler
	);

    

  private:
	std::atomic<quint64> generation{ 0 };

	std::atomic<bool> bound{ false };

	TMqttClient::SharedPtr client = nullptr;

  public:
	explicit GMqttAdapter(QObject* parent = nullptr);
};
}  // namespace gentau