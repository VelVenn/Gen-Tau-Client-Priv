#include <qprotobufserializer.h>
#include <qt6/QtCore/qcontainerfwd.h>
#include "comm/TMqttClient.hpp"
#include "hero_deploy_vt_recv.hpp"

#include "img_trans/vid_render/TBytesVidRender.hpp"

#include "utils/TLog.hpp"

#include "hdvt.qpb.h"

#include <QGuiApplication>
#include <QObject>
#include <QProtobufSerializer>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRunnable>

#include <chrono>
#include <fstream>
#include <thread>

#define T_LOG_TAG "[Hero DVT Test] "

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace gentau;

class InitGLCtx : public QRunnable
{
	TBytesVidRender* bVidRend;

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

	InitGLCtx(TBytesVidRender* _bVidRend) : bVidRend(_bVidRend) {};
};

class TestSender
{
  private:
	TMqttClient::SharedPtr client;

	QProtobufSerializer serializer;

	const steady_clock::duration inv = 20ms;

	std::jthread txThread;

  private:
	void initTxThread()
	{
		if (!client) {
			tLogError("Test sender's client is invalid");
			return;
		}

		txThread = std::jthread([&](stop_token st) mutable {
			auto next_time = steady_clock::now();

			ifstream txFile("./res/pkt_dump.h265");

			if (!txFile) {
				tLogCritical("Unable to open video stream file for sending");
				return;
			}

			constexpr size_t CHUNK_SZ = 300;
			QByteArray       buffer(CHUNK_SZ, 0);

			while (!st.stop_requested()) {
				next_time += inv;
				std::this_thread::sleep_until(next_time);

				if (!txFile.read(reinterpret_cast<char*>(buffer.data()), CHUNK_SZ) ||
					!(txFile.gcount() > 0)) {
					break;
				}

				Gentau::Topics::CustomByteBlock msg;

				msg.setData(
					QByteArray(buffer.constData(), static_cast<qsizetype>(txFile.gcount()))
				);

				try {
					auto payload = serializer.serialize(&msg);
					client->publish("CustomByteBlock", payload.toStdString());
					// tLogInfo("Published");
				} catch (const exception& e) {
					tLogError("Error when publishing video stream: {}", e.what());
				}
			}

			tLogInfo("End of video stream");
		});
	}

  public:
	void startPub() { initTxThread(); }

  public:
	TestSender() : client(TMqttClient::create("1145", CLIENT_URI)) { client->connect(); }
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
		bVidRend = TBytesVidRender::create(262'144);
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

	auto       vtRecv = new VTRecv(bVidRend, TMqttClient::create("1", CLIENT_URI), &engine);
	TestSender testSender;

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

	rootObject->scheduleRenderJob(
		new InitGLCtx(bVidRend.get()), QQuickWindow::BeforeSynchronizingStage
	);

#if defined(USE_LOCAL) && USE_LOCAL == 1
	testSender.startPub();
#endif

	return app.exec();
}