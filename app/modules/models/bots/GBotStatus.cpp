#include "bots/GBotStatus.hpp"

namespace gentau {
GBotStatus::GBotStatus(GMqttAdapter& client, QObject* parent) :
	QObject(parent),
	_client(client),
	_commonStatus(_serializer, _client)
{}
}  // namespace gentau