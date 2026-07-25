#include "models/global_status/GGlobalStatusModel.hpp"

#include "utils/TLog.hpp"

#define T_LOG_TAG "[Global Status] "

namespace gentau {
void GGlobalStatusModel::updateUnitStatus(const GlobalUnitStatus& status)
{
	if (_unitStatus != status) {
		_unitStatus = status;
		Q_EMIT unitStatusChanged(_unitStatus);
	}
}

void GGlobalStatusModel::updateLogisticsStatus(const GlobalLogisticsStatus& status)
{
	if (_logisticsStatus != status) {
		_logisticsStatus = status;
		Q_EMIT logisticsStatusChanged(_logisticsStatus);
	}
}

void GGlobalStatusModel::updateOurFortOccupiedSec(quint32 sec)
{
	if (_ourFortOccupiedSec != sec) {
		_ourFortOccupiedSec = sec;
		Q_EMIT ourFortOccupiedSecChanged(_ourFortOccupiedSec);
	}
}

void GGlobalStatusModel::updateTheirFortOccupiedSec(quint32 sec)
{
	if (_theirFortOccupiedSec != sec) {
		_theirFortOccupiedSec = sec;
		Q_EMIT theirFortOccupiedSecChanged(_theirFortOccupiedSec);
	}
}

void GGlobalStatusModel::parseUnitStatus(const QByteArray& data)
{
	GlobalUnitStatus status;

	if (!status.deserialize(&serializer(), data)) {
		tLogWarn(
			"Failed to parse GlobalUnitStatus: {}", serializer().lastErrorString().toStdString()
		);
		return;
	}

	updateUnitStatus(status);
}

void GGlobalStatusModel::parseLogisticsStatus(const QByteArray& data)
{
	GlobalLogisticsStatus status;

	if (!status.deserialize(&serializer(), data)) {
		tLogWarn(
			"Failed to parse GlobalLogisticsStatus: {}",
			serializer().lastErrorString().toStdString()
		);
		return;
	}

	updateLogisticsStatus(status);
}

void GGlobalStatusModel::parseSpecialMechanism(const QByteArray& data)
{
	GlobalSpecialMechanism status;

	if (!status.deserialize(&serializer(), data)) {
		tLogWarn(
			"Failed to parse GlobalSpecialMechanism: {}",
			serializer().lastErrorString().toStdString()
		);
		return;
	}

	auto ids     = status.mechanismId();
	auto seconds = status.mechanismTimeSec();

	if (ids.size() != seconds.size()) {
		tLogWarn(
			"GlobalSpecialMechanism data size mismatch: mechanismId size = {}, "
			"mechanismTimeSec size = {}",
			ids.size(),
			seconds.size()
		);
		return;
	}

	qint32 ourFortSec   = 0;
	qint32 theirFortSec = 0;

	for (qsizetype i = 0; i < ids.size(); i++) {
		quint32 id  = ids.at(i);
		qint32  sec = seconds.at(i);

		if (sec < 0) {
			tLogWarn(
				"GlobalSpecialMechanism contains negative time: "
				"mechanismId = {}, timeSec = {}",
				id,
				sec
			);
			continue;
		}

		switch (id) {
			case 1:
				ourFortSec = static_cast<quint32>(sec);
				break;
			case 2:
				theirFortSec = static_cast<quint32>(sec);
				break;
			default:
				// tLogWarn("GlobalSpecialMechanism contains unknown mechanismId: {}", id);
				break;
		}
	}

	updateOurFortOccupiedSec(ourFortSec);
	updateTheirFortOccupiedSec(theirFortSec);
}

void GGlobalStatusModel::resetStatus()
{
	updateUnitStatus({});
	updateLogisticsStatus({});
	updateOurFortOccupiedSec(0);
	updateTheirFortOccupiedSec(0);
}

void GGlobalStatusModel::onBindingChanged(const QString&, const QString&, quint64)
{
	client().registerTopic(this, "GlobalUnitStatus", [this](const QByteArray& data) {
		parseUnitStatus(data);
	});

	client().registerTopic(this, "GlobalLogisticsStatus", [this](const QByteArray& data) {
		parseLogisticsStatus(data);
	});

	client().registerTopic(this, "GlobalSpecialMechanism", [this](const QByteArray& data) {
		parseSpecialMechanism(data);
	});
}
}  // namespace gentau