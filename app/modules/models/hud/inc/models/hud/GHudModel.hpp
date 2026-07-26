#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "models/game_status/GGameStatusModel.hpp"
#include "models/global_status/GGlobalStatusModel.hpp"

namespace gentau {
class GHudModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(gentau::GGlobalStatusModel* globalStatus READ globalStatus CONSTANT FINAL)
	Q_PROPERTY(gentau::GGameStatusModel* gameStatus READ gameStatus CONSTANT FINAL)

  public:
	GGlobalStatusModel* globalStatus() noexcept { return &_globalStatus; }
	GGameStatusModel* gameStatus() noexcept { return &_gameStatus; }

  private:
	QProtobufSerializer _serializer;
	GMqttAdapter&       _client;

  private:
	GGlobalStatusModel _globalStatus;
	GGameStatusModel   _gameStatus;

  public:
	explicit GHudModel(GMqttAdapter& client, QObject* parent = nullptr);

	~GHudModel() override = default;
};
}  // namespace gentau