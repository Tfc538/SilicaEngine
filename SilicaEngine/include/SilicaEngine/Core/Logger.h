/**
 * @file Logger.h
 * @brief Logging system for SilicaEngine using spdlog
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * High-performance logging with engine/application separation and multiple output sinks.
 */

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>

namespace SilicaEngine {

    /// Static logging system with engine/application separation
    class Logger {
    public:
        /// Setup loggers with console and optional file output
        static void Initialize();

        /// Flush and cleanup loggers
        static void Shutdown();

        /// Get engine logger (for internal engine messages)
        static std::shared_ptr<spdlog::logger>& GetEngineLogger();

        /// Get application logger (for game/app specific messages)
        static std::shared_ptr<spdlog::logger>& GetAppLogger();

        /// Set log level (trace, debug, info, warn, err, critical)
        static void SetLogLevel(spdlog::level::level_enum level);

        /// Enable/disable file logging
        static void EnableFileLogging(bool enable, const std::string& filename = "SilicaEngine.log");

    private:
        static std::shared_ptr<spdlog::logger> s_EngineLogger;
        static std::shared_ptr<spdlog::logger> s_AppLogger;
        static bool s_Initialized;
    };

} // namespace SilicaEngine

// Logging Macros

// Engine logger macros
#ifdef SE_DEBUG
#define SE_TRACE(...)    SilicaEngine::Logger::GetEngineLogger()->trace(__VA_ARGS__)
#define SE_INFO(...)     SilicaEngine::Logger::GetEngineLogger()->info(__VA_ARGS__)
#else
#define SE_TRACE(...)    do {} while(0)
#define SE_INFO(...)     do {} while(0)
#endif
#define SE_WARN(...)     SilicaEngine::Logger::GetEngineLogger()->warn(__VA_ARGS__)
#define SE_ERROR(...)    SilicaEngine::Logger::GetEngineLogger()->error(__VA_ARGS__)
#define SE_CRITICAL(...) SilicaEngine::Logger::GetEngineLogger()->critical(__VA_ARGS__)

// Application logger macros
#ifdef SE_DEBUG
#define SE_APP_TRACE(...)    SilicaEngine::Logger::GetAppLogger()->trace(__VA_ARGS__)
#define SE_APP_INFO(...)     SilicaEngine::Logger::GetAppLogger()->info(__VA_ARGS__)
#else
#define SE_APP_TRACE(...)    do {} while(0)
#define SE_APP_INFO(...)     do {} while(0)
#endif
#define SE_APP_WARN(...)     SilicaEngine::Logger::GetAppLogger()->warn(__VA_ARGS__)
#define SE_APP_ERROR(...)    SilicaEngine::Logger::GetAppLogger()->error(__VA_ARGS__)
#define SE_APP_CRITICAL(...) SilicaEngine::Logger::GetAppLogger()->critical(__VA_ARGS__)