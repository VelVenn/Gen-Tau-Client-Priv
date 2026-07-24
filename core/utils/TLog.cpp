#include "utils/TLog.hpp"

#include <chrono>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include "spdlog/async.h"
#include "spdlog/common.h"
#include "spdlog/fmt/chrono.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace spd = spdlog;

using namespace std;

namespace gentau {
static vector<spd::sink_ptr> createSinks()
{
	static vector<spd::sink_ptr> sinks = []() {
		vector<spd::sink_ptr> s_list;

		spd::init_thread_pool(8192, 1);

		if constexpr (conf::TLogToConsole) {
			auto stdout_sink = make_shared<spd::sinks::stdout_color_sink_mt>();
			stdout_sink->set_level(spd::level::debug);
			s_list.push_back(stdout_sink);
		}

		if constexpr (conf::TLogToFile) {
#ifdef __linux__
			auto now_time_t = chrono::zoned_time<chrono::seconds>{
				chrono::current_zone(), chrono::floor<chrono::seconds>(chrono::system_clock::now())
			};
			auto local_time = now_time_t.get_local_time();
#else
			auto now_time_t = chrono::system_clock::to_time_t(chrono::system_clock::now());
			auto local_time = *localtime(&now_time_t);
#endif
			auto log_file = fmt::format("logs/gt_{:%Y-%m-%d_%H:%M:%S}.log", local_time);

			auto file_sink = make_shared<spd::sinks::basic_file_sink_mt>(log_file);

			file_sink->set_level(spd::level::trace);
			s_list.push_back(file_sink);
		}

		return s_list;
	}();  // Return value of lambda to static variable immediately.

	return sinks;
}

static LoggerPtr createAsyncLogger(const string& logger_name)
{
	auto sinks = createSinks();

	auto logger = make_shared<spd::async_logger>(
		logger_name,
		begin(sinks),
		end(sinks),
		spd::thread_pool(),
		spd::async_overflow_policy::overrun_oldest
	);

	logger->set_level(static_cast<spd::level::level_enum>(SPDLOG_ACTIVE_LEVEL));

	logger->flush_on(spd::level::err);

	spd::register_logger(logger);
	return logger;
}

LoggerPtr getImgTransLogger()
{
	static LoggerPtr logger = createAsyncLogger("ImgTrans");
	return logger;
}

LoggerPtr getCommLogger()
{
	static LoggerPtr logger = createAsyncLogger("Comm");
	return logger;
}

LoggerPtr getGeneralLogger()
{
	static LoggerPtr logger = createAsyncLogger("General");
	return logger;
}

}  // namespace gentau