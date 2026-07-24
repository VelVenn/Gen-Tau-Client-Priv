#include "runtime/GAppContext.hpp"

#include "adapter/mqtt/GMqttAdapter.hpp"
#include "utils/TLog.hpp"

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
	_connService(*_imgTrans, *_deployVtRender, _client)
{
	connect(
		&_client,
		&GMqttAdapter::bindingChanged,
		this,
		[this](const QString& newId, const QString& newUri, quint64 newGen) {
			_clientGen.store(newGen);
		},
		Qt::QueuedConnection
	);
}
}  // namespace gentau
