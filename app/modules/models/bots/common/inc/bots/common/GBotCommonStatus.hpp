#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "message.qpb.h"

#include "models/base_model/GBaseModel.hpp"

namespace gentau {
class GBotCommonStatus : public GBaseModel
{
	Q_OBJECT

	// clang-format off
    Q_PROPERTY(
        gentau::RobotStaticStatus staticStatus 
        READ staticStatus 
        NOTIFY staticStatusChanged 
        FINAL
    )
    Q_PROPERTY(
        gentau::RobotDynamicStatus dynamicStatus 
        READ dynamicStatus 
        NOTIFY dynamicStatusChanged 
        FINAL
    )
    Q_PROPERTY(
        gentau::RobotModuleStatus modStatus 
        READ modStatus 
        NOTIFY modStatusChanged 
        FINAL
    )
    Q_PROPERTY(
        gentau::RobotPerformanceSelectionSync performanceSelection 
        READ performanceSelection 
        NOTIFY performanceSelectionChanged 
        FINAL
    )
	// clang-format on

  public:
	enum class ShooterPerformance : quint32
	{
		CoolFirst = 1,
		BurstFirst,
		HeroMelee,
		HeroRanged,
	};
	Q_ENUM(ShooterPerformance)

	enum class ChassisPerformance : quint32
	{
		HpFirst = 1,
		PowerFirst,
		HeroMelee,
		HeroRanged,
	};
	Q_ENUM(ChassisPerformance)

	enum class SentryControl : quint32
	{
		Auto = 0,
		SemiAuto
	};
	Q_ENUM(SentryControl)

  Q_SIGNALS:
	void staticStatusChanged(const RobotStaticStatus& newStatus);
	void dynamicStatusChanged(const RobotDynamicStatus& newStatus);
	void modStatusChanged(const RobotModuleStatus& newStatus);
	void performanceSelectionChanged(const RobotPerformanceSelectionSync& newSelection);

  public:
	RobotStaticStatus             staticStatus() const noexcept { return _staticStatus; }
	RobotDynamicStatus            dynamicStatus() const noexcept { return _dynamicStatus; }
	RobotModuleStatus             modStatus() const noexcept { return _modStatus; }
	RobotPerformanceSelectionSync performanceSelection() const noexcept
	{
		return _performanceSelection;
	}

  private:
	void updateStaticStatus(const RobotStaticStatus& status);
	void updateDynamicStatus(const RobotDynamicStatus& status);
	void updateModStatus(const RobotModuleStatus& status);
	void updatePerformanceSelection(const RobotPerformanceSelectionSync& selection);

	void parseStaticStatus(const QByteArray& data);
	void parseDynamicStatus(const QByteArray& data);
	void parseModStatus(const QByteArray& data);
	void parsePerformanceSelection(const QByteArray& data);

	void resetStatus() override;
	void onBindingChanged(const QString&, const QString&, quint64) override;

  private:
	RobotStaticStatus             _staticStatus;
	RobotDynamicStatus            _dynamicStatus;
	RobotModuleStatus             _modStatus;
	RobotPerformanceSelectionSync _performanceSelection;

  public:
	using GBaseModel::GBaseModel;
	~GBotCommonStatus() override = default;
};
}  // namespace gentau