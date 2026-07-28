#pragma once

#include <QPointer>
#include <QString>

#include <memory>

class QQuickItem;

namespace gentau {
class TBytesVidRender;
class GMqttAdapter;
class GInputEventDispatcher;
class GBotModel;
class GBotCommonStatus;

class GBotFactory
{
  public:
	struct InitPack
	{
		TBytesVidRender&       deployVt;
		GMqttAdapter&          client;
		GInputEventDispatcher& inputCtrl;
		QPointer<QQuickItem>   imgTransQItem;
		QPointer<QQuickItem>   deployVtQItem;
	};

  public:
	std::unique_ptr<GBotModel> createBotModel(
		const QString& clientId, quint64 curGen, GBotCommonStatus& commonStatus
	);

  private:
	QPointer<QQuickItem> _imgTransQItem;
	QPointer<QQuickItem> _deployVtQItem;

	TBytesVidRender&       _deployVt;
	GMqttAdapter&          _client;
	GInputEventDispatcher& _inputCtrl;

  public:
	explicit GBotFactory(InitPack deps);
	~GBotFactory() = default;

	GBotFactory(const GBotFactory&)            = delete;
	GBotFactory& operator=(const GBotFactory&) = delete;
	GBotFactory(GBotFactory&&)                 = delete;
	GBotFactory& operator=(GBotFactory&&)      = delete;
};
}  // namespace gentau
