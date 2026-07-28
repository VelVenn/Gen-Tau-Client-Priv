#pragma once

#include <QObject>
#include <QPointer>
#include <QProtobufSerializer>
#include <QQuickItem>
#include <QVariantAnimation>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "message.qpb.h"

#include "bots/common/GBotCommonStatus.hpp"
#include "bots/common/GBotModel.hpp"

#include "img_trans/vid_render/TBytesVidRender.hpp"

#include "input/GInputEventDispatcher.hpp"

#include <chrono>
#include <optional>

namespace gentau {
class GHeroModel : public GBotModel
{
	Q_OBJECT

	Q_PROPERTY(bool isDeployVt READ isDeployVt NOTIFY isDeployVtChanged FINAL)
	Q_PROPERTY(bool isDeployMode READ isDeployMode NOTIFY isDeployModeChanged FINAL)

	Q_PROPERTY(bool isJPressed READ isJPressed NOTIFY isJPressedChanged FINAL)
	Q_PROPERTY(bool isHPressed READ isHPressed NOTIFY isHPressedChanged FINAL)
	Q_PROPERTY(bool isKPressed READ isKPressed NOTIFY isKPressedChanged FINAL)
	Q_PROPERTY(bool isLPressed READ isLPressed NOTIFY isLPressedChanged FINAL)

	Q_PROPERTY(
		double deployModeProgress READ deployModeProgress NOTIFY deployModeProgressChanged FINAL
	)

  Q_SIGNALS:
	void isDeployVtChanged(bool isDeployVt);
	void isDeployModeChanged(bool isDeployMode);
	void isJPressedChanged(bool isJPressed);
	void isHPressedChanged(bool isHPressed);
	void isKPressedChanged(bool isKPressed);
	void isLPressedChanged(bool isLPressed);
	void deployModeProgressChanged(double deployModeProgress);

  public:
	struct InitPack
	{
		GMqttAdapter&          client;
		quint64                curGen;
		GBotCommonStatus&      commonStatus;
		TBytesVidRender&       deployVt;
		GInputEventDispatcher& inputCtrl;
		QPointer<QQuickItem>   imgTransQItem;
		QPointer<QQuickItem>   deployVtQItem;
	};

	using TimePoint = std::chrono::steady_clock::time_point;

  private:
	static constexpr quint32 pressDeployModeTimeOut = 500;  // ms

  public:
	bool   isDeployVt() const noexcept;
	bool   isDeployMode() const noexcept;
	bool   isJPressed() const noexcept;
	bool   isHPressed() const noexcept;
	bool   isKPressed() const noexcept;
	bool   isLPressed() const noexcept;
	double deployModeProgress() const noexcept;

  private:
	void setDeployVt(bool isDeployVt);
	void setDeployMode(bool isDeployMode);
	void setJPressed(bool isPressed);
	void setHPressed(bool isPressed);
	void setKPressed(bool isPressed);
	void setLPressed(bool isPressed);
	void setDeployModeProgress(double progress);

  private:
	bool isHeroRanged();

	void startDeployModePress(bool targetMode);
	void cancelDeployModePress();
	void publishDeployModeCommand(bool targetMode);

	void updateEffectiveDeployMode();

	void parseDeployModeStatus(const QByteArray& data);
	void parseCustomByteBlock(const QByteArray& data);

	void restartDeployVt();

  private:
	void resetStatus() override;

	void onNewKeyEvent(const GInputEventDispatcher::KeyboardEventInfo event);

  private:
	bool _isDeployVt{ false };
	bool _isDeployMode{ false };

	bool _isJPressed{ false };
	bool _isHPressed{ false };
	bool _isKPressed{ false };
	bool _isLPressed{ false };

	bool   _serverDeployMode{ false };
	double _deployModeProgress{ 0.0 };

	std::optional<bool>      _pendingDeployMode{ std::nullopt };
	std::optional<TimePoint> _lastDeployVtRestartTime{ std::nullopt };

  private:
	TBytesVidRender&       _deployVt;
	GInputEventDispatcher& _inputCtrl;

	QPointer<QQuickItem> _imgTransQItem;
	QPointer<QQuickItem> _deployVtQItem;

	QVariantAnimation _deployModeAnimation;

	QProtobufSerializer _serializer;

  public:
	explicit GHeroModel(InitPack deps);
	~GHeroModel() override;
};
}  // namespace gentau
