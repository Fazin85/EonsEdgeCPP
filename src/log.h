#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Eon
{

	class Log
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger>& GetLogger() { return logger; }

	private:
		static std::shared_ptr<spdlog::logger> logger;
	};
} // namespace Eon

#define EON_TRACE(...) ::Eon::Log::GetLogger()->trace(__VA_ARGS__)
#define EON_INFO(...) ::Eon::Log::GetLogger()->info(__VA_ARGS__)
#define EON_WARN(...) ::Eon::Log::GetLogger()->warn(__VA_ARGS__)
#define EON_ERROR(...) ::Eon::Log::GetLogger()->error(__VA_ARGS__)
#define EON_CRITICAL(...) ::Eon::Log::GetLogger()->critical(__VA_ARGS__)