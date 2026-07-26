#include "models/hud/GHudModel.hpp"

namespace gentau {
GHudModel::GHudModel(GMqttAdapter& client, QObject* parent) :
	QObject(parent),
	_client(client),
	_globalStatus(_serializer, _client),
	_gameStatus(_serializer, _client)
{}
}  // namespace gentau