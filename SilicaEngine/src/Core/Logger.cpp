/**
 * @file Logger.cpp
 * @brief Logging system implementation
 * @author Tim Gatzke <post@tim-gatzke.de>
 */

#include "SilicaEngine/Core/Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <vector>
#include <memory>

namespace SilicaEngine {

    // Static member definitions
    std::shared_ptr<spdlog::logger> Logger::s_EngineLogger;
    std::shared_ptr<spdlog::logger> Logger::s_AppLogger;
    bool Logger::s_Initialized = false;

    void Logger::Initialize() {
        if (s_Initialized) {
            return;
        }

        // Create console sinks with colors
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);

        // Create sinks vector for multi-sink loggers
        std::vector<spdlog::sink_ptr> engine_sinks{console_sink};
        std::vector<spdlog::sink_ptr> app_sinks{console_sink};

        // Create engine logger
        s_EngineLogger = std::make_shared<spdlog::logger>("SILICA", engine_sinks.begin(), engine_sinks.end());
        s_EngineLogger->set_level(spdlog::level::trace);
        s_EngineLogger->flush_on(spdlog::level::trace);

        // Create application logger
        s_AppLogger = std::make_shared<spdlog::logger>("APP", app_sinks.begin(), app_sinks.end());
        s_AppLogger->set_level(spdlog::level::trace);
        s_AppLogger->flush_on(spdlog::level::trace);

        // Set custom pattern
        std::string pattern = "%^[%T] [%n] [%l] %v%$";
        s_EngineLogger->set_pattern(pattern);
        s_AppLogger->set_pattern(pattern);

        // Register loggers
        spdlog::register_logger(s_EngineLogger);
        spdlog::register_logger(s_AppLogger);

        s_Initialized = true;

        // Log initialization success
        s_EngineLogger->info("SilicaEngine Logger initialized");
        s_AppLogger->info("Application Logger initialized");
    }

    void Logger::Shutdown() {
        if (!s_Initialized) {
            return;
        }

        if (s_EngineLogger) {
            s_EngineLogger->info("Shutting down SilicaEngine Logger");
        }

        if (s_AppLogger) {
            s_AppLogger->info("Shutting down Application Logger");
        }

        // Flush all loggers
        spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {
            l->flush();
        });

        // Shutdown spdlog
        spdlog::shutdown();

        s_EngineLogger.reset();
        s_AppLogger.reset();
        s_Initialized = false;
    }

    std::shared_ptr<spdlog::logger>& Logger::GetEngineLogger() {
        if (!s_Initialized) {
            Initialize();
        }
        return s_EngineLogger;
    }

    std::shared_ptr<spdlog::logger>& Logger::GetAppLogger() {
        if (!s_Initialized) {
            Initialize();
        }
        return s_AppLogger;
    }

    void Logger::SetLogLevel(spdlog::level::level_enum level) {
        if (s_EngineLogger) {
            s_EngineLogger->set_level(level);
        }
        if (s_AppLogger) {
            s_AppLogger->set_level(level);
        }
    }

    void Logger::EnableFileLogging(bool enable, const std::string& filename) {
        if (!s_Initialized) {
            Initialize();
        }

        if (enable) {
            try {
                // Create file sink with rotation (5MB, 3 files)
                auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    filename, 1048576 * 5, 3);
                file_sink->set_level(spdlog::level::trace);

                // Add file sink to existing loggers
                s_EngineLogger->sinks().push_back(file_sink);
                s_AppLogger->sinks().push_back(file_sink);

                s_EngineLogger->info("File logging enabled: {}", filename);
                s_AppLogger->info("File logging enabled: {}", filename);
            }
            catch (const spdlog::spdlog_ex& ex) {
                if (s_EngineLogger) {
                    s_EngineLogger->error("File logging initialization failed: {}", ex.what());
                }
            }
        }
    }

} // namespace SilicaEngine