#include "bots/GBotStatus.hpp"
#include "bots/common/GBotModel.hpp"

namespace gentau {
GBotStatus::GBotStatus(GMqttAdapter& client, QObject* parent) :
	QObject(parent),
	_client(client),
	_commonStatus(_serializer, _client)
{}

GBotStatus::~GBotStatus() = default;
}  // namespace gentau