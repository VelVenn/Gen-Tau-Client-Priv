#include "bots/common/GBotModel.hpp"

namespace gentau {
GMqttAdapter::PublishResult GBotModel::publish(
	const std::string& topic, const QByteArray& payload, TMqttClient::QoS qos
)
{
	return _client.publish(_curGen, topic, payload, qos);
}

GMqttAdapter::RegisterResult GBotModel::registerTopic(
	const std::string& topic, GMqttAdapter::TopicHandler handler
)
{
    return _client.registerTopic(this, topic, std::move(handler));
}

GBotModel::GBotModel(
	GMqttAdapter& client, quint64 curGen, GBotCommonStatus& commonStatus, QObject* parent
) :
	QObject(parent),
	_client(client),
	_commonStatus(commonStatus),
	_curGen(curGen)
{
	Q_ASSERT(curGen > 0);

	connect(&_commonStatus, &GBotCommonStatus::onlineChanged, this, &GBotModel::onlineChanged);

	connect(&_client, &GMqttAdapter::connected, this, &GBotModel::onConnected);
	connect(&_client, &GMqttAdapter::connectionLost, this, [this](const QString& cause) {
		resetStatus();
		onConnectionLost(cause);
	});
	connect(&_client, &GMqttAdapter::connectionFailed, this, [this](const QString& cause) {
		resetStatus();
		onConnectionFailed(cause);
	});
}
}  // namespace gentau