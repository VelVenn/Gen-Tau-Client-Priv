#include "img_trans/vid_render/TVidUtils.hpp"

#include "utils/TLog.hpp"

#include <gst/gst.h>

#include <mutex>

#define T_LOG_TAG_IMG ""

using namespace std;

namespace gentau {
namespace vid {
string_view getIssueTypeLiteral(IssueType type) noexcept
{
	switch (type) {
		case IssueType::UNKNOWN:
			return "UNKNOWN";
		case IssueType::PIPELINE_INTERNAL:
			return "PIPELINE INTERNAL";
		case IssueType::PIPELINE_RESOURCE:
			return "PIPELINE RESOURCE";
		case IssueType::PIPELINE_STREAM:
			return "PIPELINE STREAM";
		case IssueType::PIPELINE_OTHER:
			return "PIPELINE OTHER";
		case IssueType::PUSH_FATAL:
			return "PUSH FATAL";
		case IssueType::PUSH_BUSY:
			return "PUSH BUSY";
		case IssueType::GENERIC:
			return "GENERIC";
		default:
			return "UNDEFINED";
	}
}

string_view getStateLiteral(StateType state) noexcept
{
	switch (state) {
		case StateType::NULL_STATE:
			return "NULL STATE";
		case StateType::READY:
			return "READY";
		case StateType::PAUSED:
			return "PAUSED";
		case StateType::RUNNING:
			return "RUNNING";
		default:
			return "UNDEFINED";
	}
}

IssueType convGstIssueType(u32 domain) noexcept
{
	IssueType iType;
	if (domain == GST_CORE_ERROR || domain == GST_LIBRARY_ERROR) {
		iType = IssueType::PIPELINE_INTERNAL;
	} else if (domain == GST_STREAM_ERROR) {
		iType = IssueType::PIPELINE_STREAM;
	} else if (domain == GST_RESOURCE_ERROR) {
		iType = IssueType::PIPELINE_RESOURCE;
	} else {
		iType = IssueType::PIPELINE_OTHER;
	}

	return iType;
}

StateType convGstState(i32 gstState) noexcept
{
	switch (gstState) {
		case GST_STATE_NULL:
			return StateType::NULL_STATE;
		case GST_STATE_READY:
			return StateType::READY;
		case GST_STATE_PAUSED:
			return StateType::PAUSED;
		case GST_STATE_PLAYING:
			return StateType::RUNNING;
		default:
			return StateType::NULL_STATE;
	}
}

std::optional<ParsedBusErr> issueParser(GstMessage* msg)
{
	if (!msg) { return nullopt; }

	g_autoptr(GError) err       = nullptr;
	g_autofree gchar* debugInfo = nullptr;
	IssueType         iType     = IssueType::UNKNOWN;
	string            errSrc    = msg->src ? GST_ELEMENT_NAME(msg->src) : "<unknown>";

	auto msgType      = GST_MESSAGE_TYPE(msg);
	bool isMsgTypeErr = false;

	switch (msgType) {
		case GST_MESSAGE_ERROR:
			gst_message_parse_error(msg, &err, &debugInfo);
			isMsgTypeErr = true;
			break;
		case GST_MESSAGE_WARNING:
			gst_message_parse_warning(msg, &err, &debugInfo);
		default:
			return nullopt;
	}

	if (err) {
		return ParsedBusErr{ .isError   = isMsgTypeErr,
							 .type      = convGstIssueType(err->domain),
							 .src       = errSrc,
							 .msg       = err->message ? err->message : "<none>",
							 .debugInfo = debugInfo ? debugInfo : "<none>" };
	}

	return nullopt;
}

void initGstContext(int* argc, char** argv[])
{
	static once_flag initFlag;
	call_once(initFlag, [argc, argv]() {
		gst_init(argc, argv);
		tImgTransLogInfo(
			"GStreamer context initialized. Version: {}.{}.{}",
			GST_VERSION_MAJOR,
			GST_VERSION_MINOR,
			GST_VERSION_MICRO
		);
	});
}
}  // namespace vid
}  // namespace gentau