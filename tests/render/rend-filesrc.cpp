#include "img_trans/vid_render/TVidRender.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRunnable>

#include <gst/gst.h>

#include <gst/gstelementfactory.h>
#include <memory>
#include <string>

using namespace std;

#define RENDER_GT 1
#define RENDER_GP 2

#define CUR_RENDER_T 1

#if CUR_RENDER_T == RENDER_GP
#define RENDER_T  _GstPipeData
#define LINK(...) sink_widget_link(__VA_ARGS__)
#elif CUR_RENDER_T == RENDER_GT
#define RENDER_T  gentau::TVidRender
#define LINK(...) linkSinkWidget(__VA_ARGS__)
#else
#define LINK(...) static_assert(false, "Unsupported RENDER_T type.")
#endif

typedef class _GstPipeData : public enable_shared_from_this<_GstPipeData>
{
  private:
	GstElement* pipeline;
	GstElement* src;
	GstElement* parser;
	GstElement* decoder;
	GstElement* mem_upload;
	GstElement* sink;
	GstElement* vconv;
	GstElement* fps_sink;

  public:
	using PipePtr    = shared_ptr<_GstPipeData>;
	using SharedThis = enable_shared_from_this<_GstPipeData>;

  public:
	bool play()
	{
		GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
		return ret != GST_STATE_CHANGE_FAILURE;
	}

	bool pause()
	{
		GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PAUSED);
		return ret != GST_STATE_CHANGE_FAILURE;
	}

	bool stop()
	{
		GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_NULL);
		return ret != GST_STATE_CHANGE_FAILURE;
	}

	void sink_widget_link(QQuickItem* item) { g_object_set(sink, "widget", item, NULL); }

	_GstPipeData(const string& file_path)
	{
		pipeline = gst_pipeline_new("hevc_render_pipe");
		src      = gst_element_factory_make("filesrc", "src");
		parser   = gst_element_factory_make("h265parse", "parser");

		// g_autoptr(GstRegistry) registry = gst_registry_get();
		// g_autoptr(GList) nvdec_l = gst_registry_get_feature_list_by_plugin(registry, "nvh265dec");
		// g_autoptr(GList) vadec_l = gst_registry_get_feature_list_by_plugin(registry, "vaapih265dec");

		// if (nvdec_l)
		//     decoder = gst_element_factory_make("nvh265dec", "decoder");
		// else if (vadec_l)
		//     decoder = gst_element_factory_make("vaapih265dec", "decoder");
		// else
		//     decoder = gst_element_factory_make("decodebin", "auto_det_decoder");

#if __APPLE__
		decoder = gst_element_factory_make("vtdec", "decoder");
#else
		decoder = gst_element_factory_make("nvh265dec", "decoder");
#endif

		vconv = gst_element_factory_make("glcolorconvert", "vconv");

		mem_upload = gst_element_factory_make("glupload", "mem_upload");
		sink       = gst_element_factory_make("qml6glsink", "sink");
		// fps_sink = gst_element_factory_make("fpsdisplaysink", "fps_sink");

		// g_assert(pipeline && src && parser && decoder && conv && sink);

		gst_bin_add_many(GST_BIN(pipeline), src, parser, decoder, vconv, mem_upload, sink, NULL);
		gst_element_link_many(src, parser, decoder, mem_upload, sink, NULL);

		g_object_set(sink, "sync", FALSE, NULL);
		g_object_set(src, "location", file_path.c_str(), NULL);

		GstBus* bus = gst_element_get_bus(pipeline);
		gst_bus_add_watch(
			bus,
			[](GstBus* bus, GstMessage* msg, gpointer data) -> gboolean {
				GError* err;
				gchar*  debug;
				switch (GST_MESSAGE_TYPE(msg)) {
					case GST_MESSAGE_ERROR:
						gst_message_parse_error(msg, &err, &debug);
						g_printerr("\n=========== ERROR ==========\n");
						g_printerr("Error: %s\n", err->message);
						g_printerr("Debug: %s\n", debug);
						g_printerr("============================\n");
						g_error_free(err);
						g_free(debug);
						break;
					case GST_MESSAGE_WARNING:
						gst_message_parse_warning(msg, &err, &debug);
						g_printerr("Warning: %s\n", err->message);
						g_error_free(err);
						g_free(debug);
						break;
					default:
						break;
				}
				return TRUE;
			},
			NULL
		);
		gst_object_unref(bus);
	}

	~_GstPipeData()
	{
		if (pipeline) {
			gst_element_set_state(pipeline, GST_STATE_NULL);
			gst_object_unref(pipeline);
		}
	}
} GstPipe;

class RunningTask : public QRunnable
{
  public:
	RunningTask(shared_ptr<RENDER_T> pipe_data) : m_sender(pipe_data) {}
	~RunningTask() override = default;

	void run() override { m_sender->play(); }

  private:
	shared_ptr<RENDER_T> m_sender;
};

int main(int argc, char* argv[])
{
	// qputenv("QT_QPA_PLATFORM", "xcb");                                // 强制QT使用X11合成器
	qputenv("QSG_RENDER_TIMING", "1");                    // 启用渲染时间测量
	qputenv("QSG_RENDER_LOOP", "basic");                  // 强制基础渲染循环
	qputenv("__GL_SYNC_TO_VBLANK", "0");                  // 禁用NVIDIA VSync
	qputenv("vblank_mode", "0");                          // 禁用Mesa VSync
	qputenv("_NET_WM_BYPASS_COMPOSITOR", "1");            // 绕过X11合成器以减少延迟
	qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");  // 禁用Wayland窗口装饰以减少延迟

	if (qEnvironmentVariableIsSet("WAYLAND_DISPLAY")) {
		qputenv("QT_QPA_PLATFORM", "wayland");
		qputenv("GST_GL_PLATFORM", "egl");  // 使用EGL作为GST的GL平台
		qputenv("GST_GL_WINDOW", "wayland");
	}

	gentau::vid::initGstContext(&argc, &argv);

	{
		QSurfaceFormat format = QSurfaceFormat::defaultFormat();
		format.setSwapInterval(0);  // 禁用 OpenGL 交换间隔
		QSurfaceFormat::setDefaultFormat(format);

		QGuiApplication app(argc, argv);
		QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

		shared_ptr<RENDER_T> pipe = make_shared<RENDER_T>("./res/raw_sintel_1080p_stream.h265");

		QQmlApplicationEngine engine;
		QObject::connect(
			&engine,
			&QQmlApplicationEngine::objectCreationFailed,
			&app,
			[]() { QCoreApplication::exit(-1); },
			Qt::QueuedConnection
		);
		engine.loadFromModule("Gentau.Test.Render.Filesrc", "RendTest");

		// 检查根对象
		if (engine.rootObjects().isEmpty()) { qFatal("QML load failed (Root Objects empty)."); }

		QQuickWindow* rootObject = static_cast<QQuickWindow*>(engine.rootObjects().first());

		// 关键调试：打印一下，看看到底有没有找到
		QQuickItem* videoItem = rootObject->findChild<QQuickItem*>("videoItem");
		if (!videoItem) {
			qFatal("CRITICAL: Failed to find objectName 'videoItem' in QML!");
		} else {
			qDebug() << "SUCCESS: Found videoItem:" << videoItem;
		}

		pipe->LINK(videoItem);
		// pipe->linkOverlayWidget(0, videoItem);
		// pipe->linkOverlayWidget(1, videoItem);
		rootObject->scheduleRenderJob(
			new RunningTask(pipe), QQuickWindow::BeforeSynchronizingStage
		);

		// QObject::connect(
		//     &app,
		//     &QGuiApplication::aboutToQuit,
		//     [&pipe]() {
		//         pipe->stop();
		//     });

		app.exec();  // pipe must destruct before the app or will cause segfault
					 // i.e. the pipe should be constructed after app
	}

	gst_deinit();
	return 0;
}
