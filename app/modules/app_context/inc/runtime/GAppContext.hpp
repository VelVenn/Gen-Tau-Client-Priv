#pragma once

#include <QObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRunnable>

#include "img_trans/TImgTrans.hpp"
#include "img_trans/vid_render/TBytesVidRender.hpp"

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "input/GInputEventDispatcher.hpp"

#include <atomic>
#include <memory>

namespace gentau {

namespace impl {
class GAppCompositor;
};

class GAppContext : public QObject
{
	Q_OBJECT

  private:
	class GInitVidRenderTask;

  private:
	void finalizeVidRenderersInit(bool success);

  public:
	quint64 clientGen() const noexcept { return _clientGen.load(); }

  public:
	void bindToWindow(QQuickWindow* window);

	Q_INVOKABLE void requestInputBlock(QObject* owner);
	Q_INVOKABLE void releaseInputBlock(QObject* owner);

  private:
	TImgTrans::SharedPtr       _imgTrans{ nullptr };
	TBytesVidRender::SharedPtr _deployVtRender{ nullptr };

	GMqttAdapter          _client;
	GInputEventDispatcher _inputEventDispatcher;

	std::atomic<quint64> _clientGen{ 0 };

  public:
	[[nodiscard]] static std::unique_ptr<GAppContext> create()
	{
		return std::make_unique<GAppContext>();
	}

	explicit GAppContext(QObject* parent = nullptr);
	~GAppContext() override = default;
};
}  // namespace gentau
