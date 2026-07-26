#include "bots/common/GBotCommonStatus.hpp"

#include "utils/TLog.hpp"

#include <chrono>

#define T_LOG_TAG "[Bot Common] "

using namespace std;
using namespace std::chrono_literals;

namespace gentau {
void GBotCommonStatus::updateStaticStatus(const RobotStaticStatus& status)
{
	if (_staticStatus != status) {
		_staticStatus = status;
		Q_EMIT staticStatusChanged(_staticStatus);
	}
}

void GBotCommonStatus::updateDynamicStatus(const RobotDynamicStatus& status)
{
	if (_dynamicStatus != status) {
		_dynamicStatus = status;
		Q_EMIT dynamicStatusChanged(_dynamicStatus);
	}
}

void GBotCommonStatus::updateModStatus(const RobotModuleStatus& status)
{
	if (_modStatus != status) {
		_modStatus = status;
		Q_EMIT modStatusChanged(_modStatus);
	}
}

void GBotCommonStatus::updatePerformanceSelection(const RobotPerformanceSelectionSync& selection)
{
	if (_performanceSelection != selection) {
		_performanceSelection = selection;
		Q_EMIT performanceSelectionChanged(_performanceSelection);
	}
}

void GBotCommonStatus::parseStaticStatus(const QByteArray& data)
{
	RobotStaticStatus status;

	if (!status.deserialize(&serializer(), data)) {
		tLogWarn(
			"Failed to parse RobotStaticStatus: {}", serializer().lastErrorString().toStdString()
		);
		return;
	}

	updateStaticStatus(status);
}

void GBotCommonStatus::parseDynamicStatus(const QByteArray& data)
{
	RobotDynamicStatus status;

	if (!status.deserialize(&serializer(), data)) {
		static auto lastLogTime = chrono::steady_clock::now() - 5s;

		const auto now = chrono::steady_clock::now();
		if (now - lastLogTime >= 5s) {
			tLogWarn(
				"Failed to parse RobotDynamicStatus: {}",
				serializer().lastErrorString().toStdString()
			);
			lastLogTime = now;
		}
		return;
	}

	updateDynamicStatus(status);
}

void GBotCommonStatus::parseModStatus(const QByteArray& data)
{
	RobotModuleStatus status;

	if (!status.deserialize(&serializer(), data)) {
		tLogWarn(
			"Failed to parse RobotModuleStatus: {}", serializer().lastErrorString().toStdString()
		);
		return;
	}

	updateModStatus(status);
}

void GBotCommonStatus::parsePerformanceSelection(const QByteArray& data)
{
	RobotPerformanceSelectionSync selection;

	if (!selection.deserialize(&serializer(), data)) {
		tLogWarn(
			"Failed to parse RobotPerformanceSelectionSync: {}",
			serializer().lastErrorString().toStdString()
		);
		return;
	}

	updatePerformanceSelection(selection);
}

void GBotCommonStatus::resetStatus()
{
	updateStaticStatus({});
	updateDynamicStatus({});
	updateModStatus({});
	updatePerformanceSelection({});
}

void GBotCommonStatus::onBindingChanged(const QString&, const QString&, quint64)
{
	client().registerTopic(this, "RobotStaticStatus", [this](const QByteArray& data) {
		parseStaticStatus(data);
	});

	client().registerTopic(this, "RobotDynamicStatus", [this](const QByteArray& data) {
		parseDynamicStatus(data);
	});

	client().registerTopic(this, "RobotModuleStatus", [this](const QByteArray& data) {
		parseModStatus(data);
	});

	client().registerTopic(this, "RobotPerformanceSelectionSync", [this](const QByteArray& data) {
		parsePerformanceSelection(data);
	});
}
}  // namespace gentau
