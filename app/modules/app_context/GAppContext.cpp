#include "runtime/GAppContext.hpp"

#include <QSize>

#include "adapter/mqtt/GMqttAdapter.hpp"
#include "utils/TLog.hpp"

#include <algorithm>
#include <stdexcept>
#include <string_view>

#define T_LOG_TAG "[App Context] "

using namespace std::string_view_literals;

namespace gentau {
class GAppContext::GInitVidRenderTask final : public QRunnable
{
  public:
	void run() override
	{
		Q_ASSERT(_ctx);

		bool imgTransStarted       = false;
		bool deployVtRenderStarted = false;

		if (!_imgTrans || !_deployVtRender) {
			tLogCritical("[VidRender Init Task] Invalid TImgTrans or TBytesVidRender instance");
		} else {
			imgTransStarted       = _imgTrans->renderer->play();
			deployVtRenderStarted = _deployVtRender->play();

			if (!imgTransStarted) {
				tLogCritical("[VidRender Init Task] Failed to start TImgTrans renderer");
			}

			if (!deployVtRenderStarted) {
				tLogCritical("[VidRender Init Task] Failed to start TBytesVidRender renderer");
			}
		}

		bool success = imgTransStarted && deployVtRenderStarted;

		QMetaObject::invokeMethod(
			_ctx,
			[ctx = _ctx, success] { ctx->finalizeVidRenderersInit(success); },
			Qt::QueuedConnection
		);
	}

  private:
	QPointer<GAppContext>      _ctx;
	TImgTrans::SharedPtr       _imgTrans;
	TBytesVidRender::SharedPtr _deployVtRender;

  public:
	GInitVidRenderTask(
		GAppContext* ctx, TImgTrans::SharedPtr imgTrans, TBytesVidRender::SharedPtr deployVtRender
	) :
		_ctx(ctx),
		_imgTrans(std::move(imgTrans)),
		_deployVtRender(std::move(deployVtRender))
	{}

	~GInitVidRenderTask() override = default;
};

static void tryResizeWindowOnInit(QQuickWindow* window)
{
	if (!window) { return; }

	constexpr auto singleQueuedConnection =
		static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::SingleShotConnection);

	// 首帧完成后触发一次微小 resize。
	QObject::connect(
		window,
		&QQuickWindow::frameSwapped,
		window,
		[window] {
			const QSize targetSize = window->size();
			const QSize nudgedSize{ std::max(1, targetSize.width() - 2),
									std::max(1, targetSize.height() - 2) };

			// 等缩小后的尺寸真正生效。
			QObject::connect(
				window,
				&QQuickWindow::widthChanged,
				window,
				[window, targetSize, nudgedSize](int width) {
					if (width != nudgedSize.width()) { return; }

					// 保证 Qt 至少用缩小后的尺寸渲染一帧，然后恢复。
					QObject::connect(
						window,
						&QQuickWindow::frameSwapped,
						window,
						[window, targetSize] {
							window->resize(targetSize);
							window->update();
						},
						singleQueuedConnection
					);

					window->update();
				},
				singleQueuedConnection
			);

			window->resize(nudgedSize);
			window->update();
		},
		singleQueuedConnection
	);
}

void GAppContext::finalizeVidRenderersInit(bool success)
{
	if (!success) {
		tLogCritical("Application failed to startup video renderers, exiting.");

		QCoreApplication::exit(EXIT_FAILURE);
		return;
	}
}

void GAppContext::bindToWindow(QQuickWindow* window)
{
	if (!window) {
		const auto cause = "Invalid QQuickWindow to bind video renderers: window is nullptr"sv;

		tLogCritical("{}", cause);

		throw std::runtime_error(cause.data());
	}

	QQuickItem* imgTransItem = window->findChild<QQuickItem*>("imgTrans");
	QQuickItem* deployVtItem = window->findChild<QQuickItem*>("deployVt");

	if (!imgTransItem) {
		const auto cause =
			"Invalid QQuickWindow to bind video renderers: QML item 'imgTrans' not found"sv;

		tLogCritical("{}", cause);

		throw std::runtime_error(cause.data());
	}

	if (!deployVtItem) {
		const auto cause =
			"Invalid QQuickWindow to bind video renderers: QML item 'deployVt' not found"sv;

		tLogCritical("{}", cause);

		throw std::runtime_error(cause.data());
	}

	tLogDebug("Found QML item 'imgTrans': <{}>", static_cast<const void*>(imgTransItem));
	_imgTrans->renderer->linkSinkWidget(imgTransItem);

	tLogDebug("Found QML item 'deployVt': <{}>", static_cast<const void*>(deployVtItem));
	_deployVtRender->linkSinkWidget(deployVtItem);

	window->scheduleRenderJob(
		new GInitVidRenderTask(this, _imgTrans, _deployVtRender),
		QQuickWindow::BeforeSynchronizingStage
	);

	_inputEventDispatcher.attachWindow(window);

	tryResizeWindowOnInit(window);
}

void GAppContext::requestInputBlock(QObject* owner)
{
	_inputEventDispatcher.requestInputBlock(owner);
}

void GAppContext::releaseInputBlock(QObject* owner)
{
	_inputEventDispatcher.releaseInputBlock(owner);
}

GAppContext::GAppContext(QObject* parent) :
	QObject(parent),
	_imgTrans(TImgTrans::create()),
	_deployVtRender(TBytesVidRender::create()),
	_client(),
	_inputEventDispatcher(_client),
	_connService(*_imgTrans, *_deployVtRender, _client),
	_hudModel(_client)
{
	connect(
		&_client,
		&GMqttAdapter::bindingChanged,
		this,
		[this](const QString& newId, const QString& newUri, quint64 newGen) {
			_clientGen.store(newGen);
		}
	);
}
}  // namespace gentau
