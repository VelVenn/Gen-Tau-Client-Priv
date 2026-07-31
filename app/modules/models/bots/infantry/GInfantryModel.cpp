#include "bots/infantry/GInfantryModel.hpp"

#include "message.qpb.h"

#include "utils/TLog.hpp"

#include <optional>

#define T_LOG_TAG "[Infantry Model] "

namespace gentau {
void GInfantryModel::publishPerformanceModeCmd(
	ShooterPerformance shooterMode, ChassisPerformance chassisMode
)
{
	const auto             staticStatus   = commonStatus().staticStatus();
	std::optional<quint32> curShooterMode = std::nullopt;
	std::optional<quint32> curChassisMode = std::nullopt;

	if (staticStatus.hasPerformanceSystemShooter()) {
		curShooterMode = std::make_optional<quint32>(staticStatus.performanceSystemShooter());
	}

	if (staticStatus.hasPerformanceSystemChassis()) {
		curChassisMode = std::make_optional<quint32>(staticStatus.performanceSystemChassis());
	}

	if (shooterMode >= ShooterPerformance::HeroMelee ||
		chassisMode >= ChassisPerformance::HeroMelee) {
		tLogWarn(
			"Invalid shooter or chassis performance mode provided: {}, {}",
			static_cast<quint32>(shooterMode),
			static_cast<quint32>(chassisMode)
		);
		return;
	}

	if ((curShooterMode && curShooterMode == static_cast<quint32>(shooterMode)) &&
		(curChassisMode && curChassisMode == static_cast<quint32>(chassisMode))) {
		tLogWarn("Incoming mode same as old, ignored");
		return;
	}

	RobotPerformanceSelectionCommand msg;
	msg.setShooter(static_cast<quint32>(shooterMode));
	msg.setChassis(static_cast<quint32>(chassisMode));

	const auto payload = msg.serialize(&_serializer);
	if (payload.isEmpty()) {
		tLogWarn(
			"Failed to serialize RobotPerformanceSelectionCommand: {}",
			_serializer.lastErrorString().toStdString()
		);
		return;
	}

	const auto result = publish("RobotPerformanceSelectionCommand", payload);
	if (!result.succeeded()) {
		tLogWarn(
			"Failed to publish RobotPerformanceSelectionCommand: {}", result.cause.toStdString()
		);
	}
}

GInfantryModel::GInfantryModel(InitPack deps) :
	GBotModel(deps.client, deps.curGen, deps.commonStatus)
{}
}  // namespace gentau