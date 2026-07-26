#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "message.qpb.h"

#include "models/base_model/GBaseModel.hpp"

namespace gentau {
class GGameStatusModel : public GBaseModel
{
	Q_OBJECT

	Q_PROPERTY(gentau::GameStatus roundStatus READ roundStatus NOTIFY roundStatusChanged FINAL)
	Q_PROPERTY(Stage stage READ stage NOTIFY stageChanged FINAL)
	Q_PROPERTY(EndReason endReason READ endReason NOTIFY endReasonChanged FINAL)
	Q_PROPERTY(Winner winner READ winner NOTIFY winnerChanged FINAL)

  public:
	enum class Stage : quint32
	{
		Unstarted = 0,
		Preparation,
		SelfTest,
		Countdown,
		InProgress,
		Settling
	};
	Q_ENUM(Stage)

	enum class EndReason : quint32
	{
		BaseDestroyed = 1,
		BaseHp,
		OutpostHp,
		OutpostDestroyed,
		TotalDamage,
		TotalHp,
		Draw,
		Aborted,
		RefereeDecision,
		None = 255
	};
	Q_ENUM(EndReason)

	enum class Winner : quint32
	{
		Draw = 0,
		Red,
		Blue,
		None = 255
	};
	Q_ENUM(Winner)

  Q_SIGNALS:
	void roundStatusChanged(const GameStatus& newStatus);

	void stageChanged(Stage newStage);
	void endReasonChanged(EndReason newEndReason);
	void winnerChanged(Winner newWinner);

  public:
	GameStatus roundStatus() const noexcept { return _roundStatus; }

	Stage     stage() const noexcept;
	EndReason endReason() const noexcept;
	Winner    winner() const noexcept;

	Q_INVOKABLE Stage     toStage(quint32 val) const noexcept { return valToStage(val); }
	Q_INVOKABLE EndReason toEndReason(quint32 val) const noexcept { return valToEndReason(val); }
	Q_INVOKABLE Winner    toWinner(quint32 val) const noexcept { return valToWinner(val); }

  public:
	static Stage     valToStage(quint32 val) noexcept;
	static EndReason valToEndReason(quint32 val) noexcept;
	static Winner    valToWinner(quint32 val) noexcept;

  private:
	void parseGameStatus(const QByteArray& data);

	void updateGameStatus(const GameStatus& status);

  private:
	void resetStatus() override;
	void onBindingChanged(const QString&, const QString&, quint64) override;

  private:
	GameStatus _roundStatus;

  public:
	using GBaseModel::GBaseModel;
	~GGameStatusModel() override = default;
};
}  // namespace gentau