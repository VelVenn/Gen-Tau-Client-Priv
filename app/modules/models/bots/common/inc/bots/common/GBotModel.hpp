#pragma once

#include <QObject>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "bots/common/GBotCommonStatus.hpp"

#include <string>

namespace gentau {
class GBotModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool online READ online NOTIFY onlineChanged FINAL)

  Q_SIGNALS:
	void onlineChanged(bool online);

  public:
	bool online() const noexcept { return _commonStatus.online(); }

  protected:
	virtual void resetStatus() {}

	virtual void onConnected() {}
	virtual void onConnectionLost(const QString& cause) {}
	virtual void onConnectionFailed(const QString& cause) {}

	GBotCommonStatus& commonStatus() { return _commonStatus; }

	GMqttAdapter::PublishResult publish(
		const std::string& topic,
		const QByteArray&  payload,
		TMqttClient::QoS   qos = TMqttClient::QoS::AT_LEAST_ONCE
	);

	GMqttAdapter::RegisterResult registerTopic(
		const std::string& topic, GMqttAdapter::TopicHandler handler
	);

  private:
	GMqttAdapter&     _client;
	GBotCommonStatus& _commonStatus;

	const quint64 _curGen;

  public:
	explicit GBotModel(
		GMqttAdapter&     client,
		quint64           curGen,
		GBotCommonStatus& commonStatus,
		QObject*          parent = nullptr
	);
};
}  // namespace gentau