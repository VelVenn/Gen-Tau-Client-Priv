#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "conf/version.hpp"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

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
