/**
 * @file Logger.h
 * @brief Logging system for SilicaEngine using spdlog
 * @version 1.0.0
 * 
 * Provides a comprehensive logging system with multiple log levels,
 * formatted output, and separate loggers for engine and application code.
 */

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>

namespace SilicaEngine {

    /**
     * @class Logger
     * @brief Static logging system for the engine
     * 
     * Provides static methods to access engine and application loggers.
     * The logger system uses spdlog for high-performance logging with
     * colored console output and optional file output.
     */
    class Logger {
    public:
        /**
         * @brief Initialize the logging system
         * 
         * Sets up both engine and application loggers with appropriate
         * formatting and output sinks.
         */
        static void Initialize();

        /**
         * @brief Shutdown the logging system
         * 
         * Flushes all pending log messages and releases logger resources.
         */
        static void Shutdown();

        /**
         * @brief Get the engine logger
         * @return Shared pointer to the engine logger
         * 
         * The engine logger is used for internal engine messages and
         * has a distinct color scheme for easy identification.
         */
        static std::shared_ptr<spdlog::logger>& GetEngineLogger();

        /**
         * @brief Get the application logger
         * @return Shared pointer to the application logger
         * 
         * The application logger is intended for game/application specific
         * logging and has its own color scheme.
         */
        static std::shared_ptr<spdlog::logger>& GetAppLogger();

        /**
         * @brief Set the log level for all loggers
         * @param level The spdlog log level to set
         * 
         * Available levels:
         * - trace: Very detailed debug information
         * - debug: Debug information  
         * - info: General information messages
         * - warn: Warning messages
         * - err: Error messages
         * - critical: Critical error messages
         */
        static void SetLogLevel(spdlog::level::level_enum level);

        /**
         * @brief Enable or disable file logging
         * @param enable True to enable file logging, false to disable
         * @param filename Optional filename (default: "SilicaEngine.log")
         * 
         * When enabled, all log messages will also be written to a file
         * in addition to console output.
         */
        static void EnableFileLogging(bool enable, const std::string& filename = "SilicaEngine.log");

    private:
        static std::shared_ptr<spdlog::logger> s_EngineLogger;   ///< Engine logger instance
        static std::shared_ptr<spdlog::logger> s_AppLogger;     ///< Application logger instance
        static bool s_Initialized;                              ///< Initialization state
    };

} // namespace SilicaEngine

// ============================================================================
// Logging Macros
// ============================================================================

// Engine logger macros
#define SE_TRACE(...)    SilicaEngine::Logger::GetEngineLogger()->trace(__VA_ARGS__)
#define SE_INFO(...)     SilicaEngine::Logger::GetEngineLogger()->info(__VA_ARGS__)
#define SE_WARN(...)     SilicaEngine::Logger::GetEngineLogger()->warn(__VA_ARGS__)
#define SE_ERROR(...)    SilicaEngine::Logger::GetEngineLogger()->error(__VA_ARGS__)
#define SE_CRITICAL(...) SilicaEngine::Logger::GetEngineLogger()->critical(__VA_ARGS__)

// Application logger macros
#define SE_APP_TRACE(...)    SilicaEngine::Logger::GetAppLogger()->trace(__VA_ARGS__)
#define SE_APP_INFO(...)     SilicaEngine::Logger::GetAppLogger()->info(__VA_ARGS__)
#define SE_APP_WARN(...)     SilicaEngine::Logger::GetAppLogger()->warn(__VA_ARGS__)
#define SE_APP_ERROR(...)    SilicaEngine::Logger::GetAppLogger()->error(__VA_ARGS__)
#define SE_APP_CRITICAL(...) SilicaEngine::Logger::GetAppLogger()->critical(__VA_ARGS__)