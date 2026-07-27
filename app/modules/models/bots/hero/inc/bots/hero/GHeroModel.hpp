#pragma once

#include <QObject>
#include <QProtobufSerializer>
#include <QQuickItem>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "message.qpb.h"

#include "bots/common/GBotCommonStatus.hpp"
#include "bots/common/GBotModel.hpp"

#include "img_trans/vid_render/TBytesVidRender.hpp"

#include "input/GInputEventDispatcher.hpp"

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
		QQuickItem&            imgTransQItem;
		QQuickItem&            deployVtQItem;
	};

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
	void onNewKeyEvent(const GInputEventDispatcher::KeyboardEventInfo event);

	void restartDeployVt();

	void resetStatus() override;
	void onConnected() override;

  private:
	TBytesVidRender&       _deployVt;
	GInputEventDispatcher& _inputCtrl;

	QQuickItem& _imgTransQItem;
	QQuickItem& _deployVtQItem;

	QProtobufSerializer _serializer;

  public:
	explicit GHeroModel(InitPack deps);
	~GHeroModel() override;
};
}  // namespace gentau