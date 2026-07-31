#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "bots/common/GBotCommonStatus.hpp"
#include "bots/common/GBotModel.hpp"

namespace gentau {
class GInfantryModel : public GBotModel
{
	Q_OBJECT

  public:
	struct InitPack
	{
		GMqttAdapter&     client;
		quint64           curGen;
		GBotCommonStatus& commonStatus;
	};

	using ShooterPerformance = GBotCommonStatus::ShooterPerformance;
	using ChassisPerformance = GBotCommonStatus::ChassisPerformance;

  public:
	Q_INVOKABLE void publishPerformanceModeCmd(
		ShooterPerformance shooterMode = ShooterPerformance::CoolFirst,
		ChassisPerformance chassisMode = ChassisPerformance::HpFirst
	);

  private:
	QProtobufSerializer _serializer;

  public:
	explicit GInfantryModel(InitPack deps);
	~GInfantryModel() override = default;
};
}  // namespace gentau
