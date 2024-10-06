#include "log.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <vector>

namespace Eon {
	std::shared_ptr<spdlog::logger> Log::logger;

	void
		Log::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("Eon.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		logger =
			std::make_shared<spdlog::logger>("EON", begin(logSinks), end(logSinks));
		spdlog::register_logger(logger);
		logger->set_level(spdlog::level::trace);
		logger->flush_on(spdlog::level::trace);
	}
} // namespace Eon
