#include <qnamespace.h>
#include <qobject.h>
#include "comm/TMqttClient.hpp"
#include "hero_deploy_vt_recv.hpp"

#include "img_trans/vid_render/TBytesVidRender.hpp"

#include "utils/TLog.hpp"

#include <QGuiApplication>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRunnable>

#define T_LOG_TAG "[Hero DVT Test] "

using namespace std;
using namespace gentau;

class InitGLCtx : public QRunnable
{
	TBytesVidRender::SharedPtr bVidRend;

  public:
	void run() override
	{
		if (!bVidRend) {
			tLogError("Bytes vid render module is invalid");
			return;
		}

		if (!bVidRend->play()) {
			tLogError("Failed to start the renderer");
			return;
		}
	}

	InitGLCtx(TBytesVidRender::SharedPtr _bVidRend) : bVidRend(std::move(_bVidRend)) {};
};

int main(int argc, char* argv[])
{
	TBytesVidRender::initContext(nullptr, nullptr);

	qputenv("QSG_RENDER_LOOP", "basic");
	qputenv("__GL_SYNC_TO_VBLANK", "0");
	qputenv("vblank_mode", "0");
	qputenv("_NET_WM_BYPASS_COMPOSITOR", "1");

	if (qEnvironmentVariableIsSet("WAYLAND_DISPLAY")) {
		qputenv("QT_QPA_PLATFORM", "wayland");
		qputenv("GST_GL_PLATFORM", "egl");
	}

	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setSwapInterval(0);
	QSurfaceFormat::setDefaultFormat(format);

	QGuiApplication app(argc, argv);

	QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

	TBytesVidRender::SharedPtr bVidRend;
	try {
		bVidRend = TBytesVidRender::create();
	} catch (const exception& e) {
		tLogCritical("Fatal error during bytes vid render init: {}", e.what());
		return -1;
	}

	QQmlApplicationEngine engine;
	QObject::connect(
		&engine,
		&QQmlApplicationEngine::objectCreationFailed,
		&app,
		[]() { QCoreApplication::exit(-1); },
		Qt::QueuedConnection
	);

	auto vtRecv = new VTRecv(bVidRend, TMqttClient::create("1", CLIENT_URI), &engine);

	QObject::connect(
		vtRecv,
		&VTRecv::clientSwitchRequested,
		&engine,
		[&engine](const QString& id) { VTRecv::clientSwitchHandler(engine, id); },
		Qt::QueuedConnection
	);

	engine.rootContext()->setContextProperty("vtRecv", vtRecv);
	engine.loadFromModule("Gentau.Test.HDVT", "DeployVt");

	QQuickWindow* rootObject = static_cast<QQuickWindow*>(engine.rootObjects().first());
	QQuickItem*   videoItem  = rootObject->findChild<QQuickItem*>("videoItem");

	if (!videoItem) {
		tLogCritical("Failed to find objectName 'videoItem' in QML!");
	} else {
		tLogDebug("Found Video Item, name: {}", videoItem->objectName().toStdString());
	}
	bVidRend->linkSinkWidget(videoItem);

	rootObject->scheduleRenderJob(new InitGLCtx(bVidRend), QQuickWindow::BeforeSynchronizingStage);

	return app.exec();
}