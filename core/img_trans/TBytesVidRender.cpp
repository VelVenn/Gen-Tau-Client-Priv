#include "img_trans/vid_render/TBytesVidRender.hpp"

#include "utils/TLog.hpp"
#include "utils/TLogical.hpp"

#include <gst/app/app.h>
#include <gst/gst.h>

#include <exception>
#include <future>
#include <stdexcept>
#include <stop_token>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#define T_LOG_TAG_IMG "[Video Render Bytes] "

using namespace std;

namespace gentau {
void TBytesVidRender::onDecoderPadAdded(GstElement* decoder, GstPad* new_pad, gpointer user_data)
{
	TBytesVidRender* self          = static_cast<TBytesVidRender*>(user_data);
	g_autoptr(GstElement) uploader = gst_bin_get_by_name(GST_BIN(self->fixedPipe), "bUploader");
	g_autoptr(GstPad) sink_pad     = gst_element_get_static_pad(uploader, "bSink");

	GstPadLinkReturn ret;
	g_autoptr(GstCaps) new_pad_caps = nullptr;
	GstStructure* new_pad_struct    = nullptr;
	const gchar*  new_pad_type      = nullptr;

	tImgTransLogTrace(
		"Recieved new pad '{}' from decoder '{}'", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(decoder)
	);

	if (gst_pad_is_linked(sink_pad)) {
		tImgTransLogTrace("Sink pad already linked, ignored");
		return;
	}

	new_pad_caps   = gst_pad_get_current_caps(new_pad);
	new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
	new_pad_type   = gst_structure_get_name(new_pad_struct);

	if (g_str_has_prefix(new_pad_type, "video/x-raw")) {
		ret = gst_pad_link(new_pad, sink_pad);
		if (GST_PAD_LINK_FAILED(ret)) {
			tImgTransLogError(
				"Type '{}' pad link failed with error '{}'",
				new_pad_type,
				gst_pad_link_get_name(ret)
			);
		} else {
			tImgTransLogInfo("Type '{}' pad linked successfully", new_pad_type);
		}
	} else {
		tImgTransLogTrace("Type '{}' pad ignored", new_pad_type);
	}
}

GstElement* TBytesVidRender::choosePrefDecoder(bool& isDynamic)
{
	vector<const gchar*> candidates = {
#ifdef __linux__
		// "nvh265dec",     // Nvidia
		// "vah265dec",     // VA-API (Intel/AMD)(high priority in gstreamer)
		// "vaapih265dec",  // VA-API // Use soft decode for stable
#elif defined(WIN32)
		"nvh265dec",     // Nvidia
		"d3d12h265dec",  // D3D12
		"d3d11h265dec",  // D3D11
		"qsvh265dec",    // QuickSync (Intel)
#elif defined(__APPLE__)
		"vtdec_hw",  // General VideoToolbox hardware decoder
		"vtdec",
#endif
		"avdec_h265",  // FFMPEG software decoder as fallback
	};

	for (auto name : candidates) {
		g_autoptr(GstElementFactory) factory = gst_element_factory_find(name);
		if (factory) {
			// Further verify for gstreamer plugin blacklist
			GstElement* element = gst_element_factory_create(factory, "bDecoder");

			if (element) {
				tImgTransLogTrace("Selected H.265 Decoder: '{}'", name);
				isDynamic = false;
				return element;
			}
		}
	}

	tImgTransLogWarn("No preferred decoder found, falling back to software decodebin.");

	isDynamic = true;
	return gst_element_factory_make("decodebin", "bDecoder");
}

bool TBytesVidRender::initBusThread()
{
	if (!pipeline()) {
		constexpr auto errMsg = "Pipeline is not initialized, cannot start bus thread."sv;
		tImgTransLogCritical("{}", errMsg);
		throw std::runtime_error(errMsg.data());
	}

	promise<void> threadErrPassThru;
	auto          passThruFuture = threadErrPassThru.get_future();

	// lamba捕获变量时会默认将其视为const成员
	busThread = jthread([this, passThru = std::move(threadErrPassThru)](stop_token sToken) mutable {
		g_autoptr(GstBus) bus = gst_element_get_bus(fixedPipe);

		if (!bus) {
			constexpr auto errMsg = "Failed to get bus from pipeline."sv;
			tImgTransLogCritical("{}", errMsg);
			passThru.set_exception(make_exception_ptr(runtime_error(errMsg.data())));
			return;
		}

		passThru.set_value();  // Notify main thread init success

		auto issueParser = [this](GstMessage* msg) {
			auto parsedIssueOpt = vid::issueParser(msg);
			if (parsedIssueOpt) {
				const auto& parsedIssue = *parsedIssueOpt;

				auto type    = parsedIssue.type;
				auto src     = parsedIssue.src;
				auto msg     = parsedIssue.msg;
				auto dbgInfo = parsedIssue.debugInfo;

				if (parsedIssue.isError) {
					onPipeError(type, src, msg, dbgInfo);

					tImgTransLogError("Render Engine error: {} | Debug info : {}", msg, dbgInfo);
				} else {
					onPipeWarn(type, src, msg, dbgInfo);

					tImgTransLogWarn("Render Engine warning: {} | Debug info : {}", msg, dbgInfo);
				}
			} else {
				tImgTransLogWarn(
					"Failed to parse bus message as {}. Ignoring it...", GST_MESSAGE_TYPE_NAME(msg)
				);
			}
		};

		while (!sToken.stop_requested()) {
			g_autoptr(GstMessage) msg = gst_bus_timed_pop_filtered(
				bus,
				100 * GST_MSECOND,
				static_cast<GstMessageType>(
					GST_MESSAGE_EOS | GST_MESSAGE_ERROR | GST_MESSAGE_WARNING |
					GST_MESSAGE_STATE_CHANGED
				)
			);

			if (!msg) { continue; }

			switch (GST_MESSAGE_TYPE(msg)) {
				case GST_MESSAGE_EOS: {
					tImgTransLogInfo("End of stream reached.");
					onEOS();
					break;
				}
				case GST_MESSAGE_ERROR:
				case GST_MESSAGE_WARNING: {
					issueParser(msg);
					break;
				}
				case GST_MESSAGE_STATE_CHANGED: {
					if (GST_MESSAGE_SRC(msg) == GST_OBJECT(fixedPipe)) {
						GstState oldState, newState;
						gst_message_parse_state_changed(msg, &oldState, &newState, nullptr);
						tImgTransLogTrace(
							"Pipeline state changed from '{}' to '{}'",
							gst_element_state_get_name(oldState),
							gst_element_state_get_name(newState)
						);
						onStateChanged(vid::convGstState(oldState), vid::convGstState(newState));
					}

					break;
				}
				default:
					tImgTransLogWarn(
						"Something weird happened, it should never goto busThread's default "
						"branch "
						"..."
					);  // Should never reach here
			}
		}
	});

	try {
		passThruFuture.get();
	} catch (const exception& e) {
		throw;  // Rethrow to main thread
	}

	tImgTransLogInfo("Bus thread started successfully.");
	return true;
}

bool TBytesVidRender::initPipeElements()
{
	bool linkDynamic = false;

	fixedPipe                  = gst_pipeline_new("bPipe");
	fixedSrc                   = gst_element_factory_make("appsrc", "bSrc");
	ElemRawPtr parser          = gst_element_factory_make("h265parse", "bParser");
	ElemRawPtr parseCapsFilter = gst_element_factory_make("capsfilter", "bParseCapsFilter");
	ElemRawPtr bufferQueue     = gst_element_factory_make("queue", "bBufferQueue");
	ElemRawPtr decoder         = choosePrefDecoder(linkDynamic);
	ElemRawPtr leakyQueue      = gst_element_factory_make("queue", "bLeakyQueue");
	ElemRawPtr colorConv       = gst_element_factory_make("glcolorconvert", "bColorConv");
	ElemRawPtr uploader        = gst_element_factory_make("glupload", "bUploader");
	ElemRawPtr sinkCapsFilter  = gst_element_factory_make("capsfilter", "bSinkCapsFilter");
	fixedSink                  = gst_element_factory_make("qml6glsink", "bSink");

	if (anyFalse(
			fixedPipe,
			fixedSrc,
			parser,
			parseCapsFilter,
			decoder,
			bufferQueue,
			uploader,
			colorConv,
			leakyQueue,
			sinkCapsFilter,
			fixedSink
		)) {
		for (auto elem : { fixedPipe,
						   fixedSrc,
						   parser,
						   parseCapsFilter,
						   decoder,
						   bufferQueue,
						   uploader,
						   colorConv,
						   leakyQueue,
						   sinkCapsFilter,
						   fixedSink }) {
			if (elem) { gst_object_unref(elem); }
		}

		constexpr auto errMsg = "Failed to create all Gstreamer elements."sv;
		tImgTransLogCritical("{}", errMsg);
		throw std::runtime_error(errMsg.data());
	}

	gst_bin_add_many(
		GST_BIN(fixedPipe),
		fixedSrc,
		parser,
		parseCapsFilter,
		decoder,
		bufferQueue,
		uploader,
		colorConv,
		leakyQueue,
		sinkCapsFilter,
		fixedSink,
		nullptr
	);

	constexpr auto errMsg =
		"Failed to link GStreamer elements."sv;  // string_view 在编译期被构造和分配空间

	if (linkDynamic) {
		if (anyFalse(
				gst_element_link_many(
					uploader, colorConv, sinkCapsFilter, leakyQueue, fixedSink, nullptr
				),
				gst_element_link_many(
					fixedSrc, parser, parseCapsFilter, bufferQueue, decoder, nullptr
				)
			)) {
			gst_object_unref(fixedPipe);
			tImgTransLogCritical("{}", errMsg);
			throw std::runtime_error(errMsg.data());
		}

		g_signal_connect(decoder, "pad-added", G_CALLBACK(onDecoderPadAdded), this);
		tImgTransLogTrace("Decoder will be linked dynamically");
	} else {
		if (!gst_element_link_many(
				fixedSrc,
				parser,
				parseCapsFilter,
				bufferQueue,
				decoder,
				uploader,
				colorConv,
				sinkCapsFilter,
				leakyQueue,
				fixedSink,
				nullptr
			)) {
			gst_object_unref(fixedPipe);
			tImgTransLogCritical("{}", errMsg);
			throw std::runtime_error(errMsg.data());
		}
		tImgTransLogTrace("Decoder will be linked statically");
	}

	g_object_set(fixedSink, "sync", FALSE, nullptr);

	// 指定appsrc的caps属性('video/x-265')可能会导致h265parse在解析时更严格，从而导致播放卡死，因此暂不指定caps
	g_object_set(
		fixedSrc,
		"is-live",
		TRUE,
		"min-latency",
		0,  // No latency, push frames as soon as possible
		"max-latency",
		-1,  // Send at best effort
		"do-timestamp",
		FALSE,
		"format",
		GST_FORMAT_BYTES,
		"stream-type",
		GST_APP_STREAM_TYPE_STREAM,
		"emit-signals",
		FALSE,
		"block",
		FALSE,
		nullptr
	);

	g_object_set(parser, "config-interval", -1, "disable-passthrough", TRUE, nullptr);

	g_autoptr(GstCaps) parseOutCaps = gst_caps_new_simple(
		"video/x-h265",
		"stream-format",
		G_TYPE_STRING,
		"byte-stream",
		"alignment",
		G_TYPE_STRING,
		"au",
		nullptr
	);
	g_object_set(parseCapsFilter, "caps", parseOutCaps, nullptr);

	g_object_set(bufferQueue, "max-size-buffers", 2, "leaky", 0, nullptr);
	g_object_set(
		leakyQueue,
		"max-size-buffers",
		1,  // Only keep the last frame
		"max-size-bytes",
		0,  // Disabled
		"max-size-time",
		0,  // Disabled
		"leaky",
		2,  // downstream
		nullptr
	);

	// Caps string ref: https://fossies.org/linux/gstreamer/tests/check/gst/gstcaps.c
	// Line 148:156 'non_simple_caps_string' and Line 216:228
	const gchar* sinkCapStr =
		"video/x-raw(memory:GLMemory), "
#ifdef __linux__
		"format=(string){NV12, RGBA, BGRA}, "
#elif defined(__APPLE__)
		"format=(string){RGBA, BGRA}, "
#endif
		"texture-target=(string)2D";
	g_autoptr(GstCaps) caps =
		gst_caps_from_string(sinkCapStr);  // This API's behavior is not stable under 1.20
	g_object_set(sinkCapsFilter, "caps", caps, nullptr);

	tImgTransLogInfo("Pipeline initialized successfully, ready to start bus thread.");

	bool res = initBusThread();

	return res;
}

TBytesVidRender::TBytesVidRender(u64 _maxBufferBytes)
{
	initPipeElements();
	maxBufferBytes.store(_maxBufferBytes);
}

TBytesVidRender::~TBytesVidRender()
{
	tImgTransLogInfo("Pipeline resources released, render destroyed.");

	if (fixedPipe) {
		gst_element_set_state(fixedPipe, GST_STATE_NULL);
		gst_object_unref(fixedPipe);
		fixedPipe = nullptr;
	}
}

bool TBytesVidRender::tryPushFrame(span<const u8> frameData)
{
	if (!fixedPipe || !fixedSrc) {
		tImgTransLogError("Push frame failed: Pipeline is not initialized.");
		return false;
	}

	auto curBytes = gst_app_src_get_current_level_bytes(GST_APP_SRC(fixedSrc));
	auto limit    = maxBufferBytes.load();
	if (curBytes > limit) {
		// tImgTransLogWarn(
		// 	"Current buffer level '{}' bytes exceeds the maximum threshold of '{}' bytes, skipping "
		// 	"frame push.",
		// 	curBytes,
		// 	limit
		// );
		return false;
	}

	if (frameData.empty()) {
		tImgTransLogWarn("Empty frame data, ignoring it.");

		return false;
	}

	auto buf = gst_buffer_new_memdup(frameData.data(), frameData.size());
	if (buf) {
		auto ret = gst_app_src_push_buffer(GST_APP_SRC(fixedSrc), buf);
		if (ret == GST_FLOW_OK) {
			lastPushSuccess.store(chrono::steady_clock::now());
			return true;
		}
	}

	return false;
}

bool TBytesVidRender::play()
{
	if (!pipeline()) {
		tImgTransLogError("Play failed: Pipeline is not initialized.");
		return false;
	}

	GstStateChangeReturn ret = gst_element_set_state(fixedPipe, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		tImgTransLogError("Failed to set pipeline to PLAYING state.");
		return false;
	}
	tImgTransLogInfo("Pipeline set to PLAYING state.");
	return true;
}

bool TBytesVidRender::pause()
{
	if (!pipeline()) {
		tImgTransLogError("Pause failed: Pipeline is not initialized.");
		return false;
	}

	GstStateChangeReturn ret = gst_element_set_state(fixedPipe, GST_STATE_PAUSED);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		tImgTransLogError("Failed to set pipeline to PAUSED state.");
		return false;
	}
	tImgTransLogInfo("Pipeline set to PAUSED state.");
	return true;
}

// reset() 和 stopPipeline() 是硬件资源级的重置，在MacOS上如果依赖vtdec_hw系列的硬件解码器，这两个API可能会导致严重错误
// 仅保证在Linux系统下的稳定性，其他平台应谨慎使用
bool TBytesVidRender::restart()
{
	if (!pipeline()) {
		tImgTransLogError("Reset failed: Pipeline is not initialized.");
		return false;
	}

	GstStateChangeReturn ret = gst_element_set_state(fixedPipe, GST_STATE_NULL);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		tImgTransLogError("Failed to reset pipeline");
		return false;
	}

	if (!play()) {
		tImgTransLogError("Failed to restart pipeline after reset");
		return false;
	}

	tImgTransLogInfo("Pipeline reset success");
	return true;
}

bool TBytesVidRender::stop()
{
	if (!pipeline()) {
		tImgTransLogError("Stop failed: Pipeline is not initialized.");
		return false;
	}

	GstStateChangeReturn ret = gst_element_set_state(fixedPipe, GST_STATE_NULL);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		tImgTransLogError("Failed to stop pipeline");
		return false;
	}
	return true;
}

bool TBytesVidRender::flush()
{
	if (!pipeline() || !src()) {
		tImgTransLogError("Flush failed: Pipeline is not initialized.");
		return false;
	}

	if (!gst_element_send_event(fixedSrc, gst_event_new_flush_start())) {
		tImgTransLogError("Failed to send FLUSH START event.");
		return false;
	}

	if (!gst_element_send_event(fixedSrc, gst_event_new_flush_stop(TRUE))) {
		tImgTransLogError("Failed to send FLUSH STOP event.");
		return false;
	}

#ifdef __APPLE__
	g_autoptr(GstCaps) currentCaps = nullptr;
	g_object_get(fixedSrc, "caps", &currentCaps, nullptr);

	// 重新设置它，这会欺骗解码器让它以为流变了，从而重置硬件会话
	if (currentCaps) { g_object_set(fixedSrc, "caps", currentCaps, nullptr); }
#endif

	tImgTransLogInfo("Pipeline flushed successfully.");
	return true;
}

vid::StateType TBytesVidRender::getCurrentState()
{
	GstState state;
	gst_element_get_state(fixedPipe, &state, nullptr, 0);
	return vid::convGstState(state);
}

void TBytesVidRender::linkSinkWidget(QQuickItem* widget)
{
	g_object_set(fixedSink, "widget", widget, nullptr);
}

void TBytesVidRender::initContext(int* argc, char** argv[])
{
	vid::initGstContext(argc, argv);
}
}  // namespace gentau