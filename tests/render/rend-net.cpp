#include "img_trans/TImgTrans.hpp"

#include "utils/TLog.hpp"

#include <qquickwindow.h>
#include <qrunnable.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRunnable>

#include <exception>
#include <memory>

using namespace gentau;
using namespace std;

#define T_LOG_TAG "[Render Test Rend-Net] "

class RunningTask : public QRunnable
{
  private:
	shared_ptr<TImgTrans> testTrans;

  public:
	RunningTask(shared_ptr<TImgTrans> testTrans) : testTrans(testTrans) {}
	~RunningTask() override = default;

	void run() override
	{
		if (!testTrans) {
			tLogError("The TImgTrans ptr cannot be null");
			return;
		}

		if (!testTrans->renderer->play()) {
			tLogError("Failed to start rendering");
			return;
		}
	}
};

//! ==========================================================================
//!                          !!! ATTENTION !!!
//! 使用 TImgTrans 图传模块时，必须严格按照以下步骤 （1）- （11） 来初始化和使用组件，任
//! 何步骤的遗漏或顺序错误都可能导致严重的性能问题、渲染问题，甚至程序崩溃 ！！！
//!
//!                      >>> EXTRA EXPLANATION <<<
//! Step (1) - (3): 必须在 QGuiApplication 实例化之前彻底锁定底层渲染环境
//! Step (4): QGuiApplication 的实例化必须先于 TImgTrans 的创建，因为 TImgTrans
//!           的内部组件 （TVidRender）运行时依赖 Qt 的组件与上下文环境。否则必然会导致
//!           QGuiApplication 最终先于 TVidRender 析构，TVidRender 在访问已被销毁
//!           的 Qt 组件而引发段错误
//! Step (5): 强制使用 OpenGL 渲染以确保与 TVidRender 的渲染管道完全兼容
//! Step (9): 必须用 scheduleRenderJob 来安排渲染任务，确保在 QML 界面 (根部件) 渲染
//!           同步阶段之前调用 TVidRender::play(), 实现 Qt 与 Gstreamer 的 OpenGL
//!           上下文共享，确保渲染器能够正确地将视频帧渲染到 QML 界面上，避免未定义行为和
//!           潜在的段错误
//! ==========================================================================

int main(int argc, char* argv[])
{
	vid::initGstContext(&argc, &argv);  // 初始化Gstreamer上下文 (1)

	// 设置环境变量以优化渲染性能和减少延迟 (2)
	qputenv("QSG_RENDER_TIMING", "1");                    // 启用渲染时间测量
	qputenv("QSG_RENDER_LOOP", "basic");                  // 强制基础渲染循环
	qputenv("__GL_SYNC_TO_VBLANK", "0");                  // 禁用NVIDIA VSync (if use NVIDIA GPU)
	qputenv("vblank_mode", "0");                          // 禁用Mesa VSync (if use Mesa drivers)
	qputenv("_NET_WM_BYPASS_COMPOSITOR", "1");            // 绕过X11合成器以减少延迟
	qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");  // 禁用Wayland窗口装饰以减少延迟

	if (qEnvironmentVariableIsSet("WAYLAND_DISPLAY")) {
		qputenv("QT_QPA_PLATFORM", "wayland");  // 强制QT使用Wayland平台 (如果在Wayland环境下)
		qputenv("GST_GL_PLATFORM", "egl");      // 使用EGL作为GST的GL平台
	}
	// 仅供参考，但是环境变量的设置必须要在 QGuiApplication 实例化之前进行，否则可能无法生效，甚至导致不稳定行为。
	// ----------------------------- (2)

	// 禁用 OpenGL 交换间隔 (3)
	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setSwapInterval(0);
	QSurfaceFormat::setDefaultFormat(format);
	// ------------------ (3)

	QGuiApplication app(argc, argv);  // QGuiApplication 必须先于 TimgTrans 创建 (4)
	QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);  // 强制使用OpenGL渲染 (5)

	// 创建 TImgTrans 实例 (6)
	TImgTrans::SharedPtr imgTrans;

	try {
		imgTrans = TImgTrans::create();
	} catch (const std::exception& e) {
		tLogCritical("Fatal error during img trans init: {}", e.what());
		return -1;
	}
	// ----------------- (6)

	// 初始化 QML 界面并连接信号 (7)
	QQmlApplicationEngine engine;
	QObject::connect(
		&engine,
		&QQmlApplicationEngine::objectCreationFailed,
		&app,
		[]() { QCoreApplication::exit(-1); },
		Qt::QueuedConnection
	);
	engine.loadFromModule("Gentau.Test.Render.Net", "RendTest");
	// --------------------- (7)

	if (engine.rootObjects().isEmpty()) { qFatal("QML load failed (Root Objects empty)."); }

	// 查找 QML 中的 videoItem 并将其链接到渲染器 (8)
	QQuickWindow* rootObject = static_cast<QQuickWindow*>(engine.rootObjects().first());

	QQuickItem* videoItem = rootObject->findChild<QQuickItem*>("videoItem");
	if (!videoItem) {
		qFatal("CRITICAL: Failed to find objectName 'videoItem' in QML!");
	} else {
		qDebug() << "SUCCESS: Found videoItem:" << videoItem;
	}

	imgTrans->renderer->linkSinkWidget(videoItem);
	// ------------------------------------ (8)

	// 在 Qt 渲染同步阶段之前安排 TImgTrans 渲染任务 (9)
	rootObject->scheduleRenderJob(
		new RunningTask(imgTrans), QQuickWindow::BeforeSynchronizingStage
	);

	imgTrans->receiver->start();  // 启动网络接收线程 (10)

	return app.exec();  // 进入 Qt 事件循环 (11)
}