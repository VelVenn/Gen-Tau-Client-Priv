#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "models/global_status/GGlobalStatusModel.hpp"

namespace gentau {
class GHudModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(gentau::GGlobalStatusModel* globalStatus READ globalStatus CONSTANT FINAL)

  private:
	GGlobalStatusModel* globalStatus() noexcept { return &_globalStatus; }

  private:
	QProtobufSerializer _serializer;
	GMqttAdapter&       _client;

  private:
	GGlobalStatusModel _globalStatus;

  public:
	explicit GHudModel(
		GMqttAdapter& client, QObject* parent = nullptr
	);

	~GHudModel() override = default;
};
}  // namespace gentau