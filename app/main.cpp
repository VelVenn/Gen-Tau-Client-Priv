#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSurfaceFormat>

#include "conf/version.hpp"
#include "img_trans/vid_render/TVidUtils.hpp"
#include "utils/TLog.hpp"

#include "GAppQmlContext.hpp"
#include "runtime/GAppContext.hpp"

#define T_LOG_TAG "[App Init] "

using namespace gentau;

int main(int argc, char* argv[])
{
	// qputenv("QSG_RENDER_LOOP", "basic");
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

	vid::initGstContext(&argc, &argv);

	QGuiApplication app(argc, argv);
	QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

	std::unique_ptr<GAppContext> appContext = nullptr;

	try {
		appContext = GAppContext::create();
	} catch (const std::exception& e) {
		tLogCritical("Failed to create GAppContext: {}", e.what());
		return EXIT_FAILURE;
	}

	GAppQmlContext::setInstance(appContext.get());

	app.setOrganizationName("Taurus");
	app.setOrganizationDomain("taurus.io");
	app.setApplicationName("gen-tau");

	QQmlApplicationEngine engine;
	QObject::connect(
		&engine,
		&QQmlApplicationEngine::objectCreationFailed,
		&app,
		[]() { QCoreApplication::exit(-1); },
		Qt::QueuedConnection
	);
	engine.loadFromModule(GT_QML_MOD_URI_PREFIX, "Main");

	if (engine.rootObjects().isEmpty()) {
		tLogCritical("Failed to load QML root object");
		return EXIT_FAILURE;
	}

	QQuickWindow* rootWindow = qobject_cast<QQuickWindow*>(engine.rootObjects().first());

	try {
		appContext->bindToWindow(rootWindow);
	} catch (const std::exception& e) {
		tLogCritical("Failed to bind app context to window: {}", e.what());
		return EXIT_FAILURE;
	}

	return app.exec();
}
