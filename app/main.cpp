#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQuickItem>
#include <QSurfaceFormat>

#include "conf/version.hpp"

int main(int argc, char* argv[])
{
	qputenv("QSG_RENDER_LOOP", "basic");
	qputenv("__GL_SYNC_TO_VBLANK", "0");
	qputenv("vblank_mode", "0");
	qputenv("_NET_WM_BYPASS_COMPOSITOR", "1");

#ifdef Q_OS_LINUX
	// qputenv("QT_QPA_PLATFORM", "xcb");
	// qputenv("GST_GL_WINDOW", "x11");
	// qputenv("GST_GL_PLATFORM", "glx");
#endif

	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setSwapInterval(0);
	QSurfaceFormat::setDefaultFormat(format);

	QGuiApplication app(argc, argv);

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

	return app.exec();
}
