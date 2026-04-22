#pragma once

#include "utils/TSignal.hpp"
#include "utils/TTypeRedef.hpp"

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

extern "C"
{
	struct _GstElement;
	typedef struct _GstElement GstElement;

	struct _GstPad;
	typedef struct _GstPad GstPad;

	struct _GstMessage;
	typedef struct _GstMessage GstMessage;

	typedef void* gpointer;
}

namespace gentau {
namespace vid {
using Frame     = std::vector<u8>;
using FramePtr  = std::unique_ptr<Frame>;
using TimePoint = std::chrono::steady_clock::time_point;

using ElemRawPtr = GstElement*;

enum class IssueType : u32
{
	UNKNOWN = 0,

	PIPELINE_INTERNAL,  // GStreamer pipeline internal error
	PIPELINE_RESOURCE,
	PIPELINE_STREAM,
	PIPELINE_OTHER,

	PUSH_FATAL,  // Issues relate to pushing frames
	PUSH_BUSY,

	GENERIC  // Generic issue, not directly from GStreamer
};

enum class StateType : u8
{
	NULL_STATE = 0,
	READY,
	PAUSED,
	RUNNING
};

template<typename OwnerT>
// `gentau::vid::IssueType`, `std::string` (src), `std::string` (msg), `std::string` (debug info)
using BusErrSignal = TSignal<
	OwnerT,
	IssueType,
	std::string /*src*/,
	std::string /*msg*/,
	std::string /*Debug info*/>;

template<typename OwnerT>
// `gentau::vid::StateType` (old state), `gentau::vid::StateType` (new state)
using BusStateChangeSignal = TSignal<OwnerT, StateType /*Old state*/, StateType /*New state*/>;

struct ParsedBusErr
{
	bool        isError   = false;  // true for error, false for warning
	IssueType   type      = IssueType::UNKNOWN;
	std::string src       = "<unknown>";
	std::string msg       = "<none>";
	std::string debugInfo = "<none>";
};

std::string_view            getIssueTypeLiteral(IssueType type) noexcept;
IssueType                   convGstIssueType(u32 domain) noexcept;
std::optional<ParsedBusErr> issueParser(GstMessage* msg);

std::string_view getStateLiteral(StateType state) noexcept;
StateType        convGstState(i32 gstState) noexcept;

void initGstContext(int* argc, char** argv[]);
}  // namespace vid
}  // namespace gentau