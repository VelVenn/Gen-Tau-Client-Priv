#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "message.qpb.h"

#include "models/base_model/GBaseModel.hpp"

namespace gentau {
class GGlobalStatusModel : public GBaseModel
{
	Q_OBJECT

	// clang-format off
	Q_PROPERTY(GlobalUnitStatus unitStatus READ unitStatus NOTIFY unitStatusChanged FINAL)

	Q_PROPERTY(
		GlobalLogisticsStatus logisticsStatus
		READ logisticsStatus
		NOTIFY logisticsStatusChanged
		FINAL
	)

	Q_PROPERTY(
		quint32 ourFortOccupiedSec READ ourFortOccupiedSec NOTIFY ourFortOccupiedSecChanged FINAL
	)

	Q_PROPERTY(
		quint32 theirFortOccupiedSec
		READ theirFortOccupiedSec
		NOTIFY theirFortOccupiedSecChanged
		FINAL
	)
	// clang-format on

  Q_SIGNALS:
	void unitStatusChanged(const GlobalUnitStatus& newStatus);
	void logisticsStatusChanged(const GlobalLogisticsStatus& newStatus);
	void ourFortOccupiedSecChanged(quint32 newSec);
	void theirFortOccupiedSecChanged(quint32 newSec);

  public:
	GlobalUnitStatus      unitStatus() const noexcept { return _unitStatus; }
	GlobalLogisticsStatus logisticsStatus() const noexcept { return _logisticsStatus; }
	quint32               ourFortOccupiedSec() const noexcept { return _ourFortOccupiedSec; }
	quint32               theirFortOccupiedSec() const noexcept { return _theirFortOccupiedSec; }

  private:
	void parseUnitStatus(const QByteArray& data);
	void parseLogisticsStatus(const QByteArray& data);
	void parseSpecialMechanism(const QByteArray& data);

	void updateUnitStatus(const GlobalUnitStatus& status);
	void updateLogisticsStatus(const GlobalLogisticsStatus& status);

	void updateOurFortOccupiedSec(quint32 sec);
	void updateTheirFortOccupiedSec(quint32 sec);

  private:
	void resetStatus() override;

	void onBindingChanged(const QString&, const QString&, quint64) override;

  private:
	GlobalUnitStatus      _unitStatus;
	GlobalLogisticsStatus _logisticsStatus;

	quint32 _ourFortOccupiedSec{ 0 };
	quint32 _theirFortOccupiedSec{ 0 };

  public:
	using GBaseModel::GBaseModel;  // 继承 GBaseModel 的构造函数
	~GGlobalStatusModel() override = default;
};
}  // namespace gentau
