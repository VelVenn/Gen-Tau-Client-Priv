#include "models/base_model/GBaseModel.hpp"

namespace gentau {
void GBaseModel::updateOnline(bool online)
{
	if (_online != online) {
		_online = online;
		Q_EMIT onlineChanged(_online);
	}
}

void GBaseModel::handleBindingChanged(const QString& newId, const QString& newUri, quint64 newGen)
{
	updateOnline(false);
	resetStatus();

	onBindingChanged(newId, newUri, newGen);
}

void GBaseModel::handleConnected()
{
	updateOnline(true);
	onConnected();
}

void GBaseModel::handleConnectionLost(const QString& cause)
{
	updateOnline(false);
	resetStatus();

	onConnectionLost(cause);
}

void GBaseModel::handleConnectionFailed(const QString& cause)
{
	updateOnline(false);
	resetStatus();

	onConnectionFailed(cause);
}

GBaseModel::GBaseModel(QProtobufSerializer& serializer, GMqttAdapter& client, QObject* parent) :
	QObject(parent),
	_serializer(serializer),
	_client(client)
{
	connect(&_client, &GMqttAdapter::bindingChanged, this, &GBaseModel::handleBindingChanged);
	connect(&_client, &GMqttAdapter::connected, this, &GBaseModel::handleConnected);
	connect(&_client, &GMqttAdapter::connectionLost, this, &GBaseModel::handleConnectionLost);
	connect(&_client, &GMqttAdapter::connectionFailed, this, &GBaseModel::handleConnectionFailed);
}
}  // namespace gentau