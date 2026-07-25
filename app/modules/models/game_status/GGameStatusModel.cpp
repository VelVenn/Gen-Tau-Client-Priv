#include "models/game_status/GGameStatusModel.hpp"

#include "message.qpb.h"
#include "utils/TLog.hpp"

#define T_LOG_TAG "[Game Status] "

namespace gentau {
auto GGameStatusModel::valToStage(quint32 val) noexcept -> Stage
{
	if (val < static_cast<quint32>(Stage::Unstarted) ||
		val > static_cast<quint32>(Stage::Settling)) {
		return Stage::Unstarted;
	}

	return static_cast<Stage>(val);
}

auto GGameStatusModel::valToEndReason(quint32 val) noexcept -> EndReason
{
	if (val < static_cast<quint32>(EndReason::BaseDestroyed) ||
		val > static_cast<quint32>(EndReason::RefereeDecision)) {
		return EndReason::None;
	}

	return static_cast<EndReason>(val);
}

auto GGameStatusModel::valToWinner(quint32 val) noexcept -> Winner
{
	if (val < static_cast<quint32>(Winner::Draw) || val > static_cast<quint32>(Winner::Blue)) {
		return Winner::None;
	}

	return static_cast<Winner>(val);
}

auto GGameStatusModel::stage() const noexcept -> Stage
{
	if (_gameStatus.hasCurrentStage()) { return toStage(_gameStatus.currentStage()); }

	return Stage::Unstarted;
}

auto GGameStatusModel::endReason() const noexcept -> EndReason
{
	if (_gameStatus.hasEndReason()) { return toEndReason(_gameStatus.endReason()); }

	return EndReason::None;
}

auto GGameStatusModel::winner() const noexcept -> Winner
{
	if (_gameStatus.hasGameResult()) { return toWinner(_gameStatus.gameResult()); }

	return Winner::None;
}

void GGameStatusModel::updateGameStatus(const GameStatus& status)
{
	if (_gameStatus == status) { return; }

	const Stage     oldStage     = stage();
	const Winner    oldWinner    = winner();
	const EndReason oldEndReason = endReason();

	_gameStatus = status;

	const Stage     newStage     = stage();
	const Winner    newWinner    = winner();
	const EndReason newEndReason = endReason();

	Q_EMIT gameStatusChanged(_gameStatus);

	if (oldStage != newStage) { Q_EMIT stageChanged(newStage); }

	if (oldWinner != newWinner) { Q_EMIT winnerChanged(newWinner); }

	if (oldEndReason != newEndReason) { Q_EMIT endReasonChanged(newEndReason); }
}

void GGameStatusModel::resetStatus()
{
	updateGameStatus({});
}

void GGameStatusModel::parseGameStatus(const QByteArray& data)
{
	GameStatus status;

	if (!status.deserialize(&serializer(), data)) {
		tLogWarn("Failed to parse GameStatus: {}", serializer().lastErrorString().toStdString());
		return;
	}

	updateGameStatus(status);
}

void GGameStatusModel::onBindingChanged(const QString&, const QString&, quint64)
{
	client().registerTopic(this, "GameStatus", [this](const QByteArray& data) {
		parseGameStatus(data);
	});
}
}  // namespace gentau