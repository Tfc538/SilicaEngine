# Logger Module

The `Logger` module provides a robust and high-performance logging system for SilicaEngine, leveraging the `spdlog` library. It offers flexible logging capabilities with separate loggers for engine-internal messages and application-specific messages, customizable output, and compile-time log level control for optimized release builds.

## Key Features

*   **Dual-Logger Architecture**:
    *   `Engine Logger`: For all internal engine-related messages.
    *   `Application Logger`: For game or application-specific messages, allowing clear separation of logs.
*   **High Performance**: Built on `spdlog`, known for its speed and low overhead.
*   **Configurable Output**:
    *   **Console Output**: Colored console output for easy readability, distinguishing between engine and application logs.
    *   **File Logging**: Optional persistent file logging with support for rotating log files to manage disk space (e.g., 5MB per file, 3 files rotation).
*   **Multiple Log Levels**: Supports standard log levels from most verbose to most critical:
    *   `TRACE`: Highly detailed information, typically for debugging.
    *   `DEBUG`: Debugging information, useful during development.
    *   `INFO`: General informational messages.
    *   `WARN`: Warning messages, indicating potential issues.
    *   `ERROR`: Error conditions that prevent normal operation.
    *   `CRITICAL`: Severe errors that may lead to application failure.
*   **Compile-Time Optimization**: In `Release` builds (when `SE_DEBUG` is not defined), `TRACE` and `INFO` level logging macros are compiled out, completely eliminating their overhead for production environments. `WARN`, `ERROR`, and `CRITICAL` logs remain active for essential runtime diagnostics.
*   **Dynamic Log Level Control**: Log levels can be adjusted at runtime for both loggers, allowing for flexible debugging without recompilation.
*   **Thread Safety**: All logging operations are thread-safe, ensuring integrity in multi-threaded applications.

## Usage

### Initialization and Shutdown

The `Logger` is a static class, so it doesn't require instantiation. It should be initialized once at application startup and shut down gracefully before application exit.

```cpp
// In your application startup
SilicaEngine::Logger::Initialize();

// In your application shutdown
SilicaEngine::Logger::Shutdown();
```

### Logging Messages

Convenient macros are provided for logging messages at different levels for both the engine and application loggers. These macros support `spdlog`'s fmtlib-style formatting.

**Engine Logging (for internal engine development):**

```cpp
SE_TRACE("Engine trace message: {}", someEngineVariable);
SE_INFO("Engine info: Initializing subsystem.");
SE_WARN("Engine warning: Resource {} not found.", resourceId);
SE_ERROR("Engine error: Failed to load asset: {}", assetPath);
SE_CRITICAL("Engine critical: Unrecoverable error in rendering pipeline!");
```

**Application Logging (for game/app specific messages):**

```cpp
SE_APP_TRACE("Player position: ({}, {}, {})", player.x, player.y, player.z);
SE_APP_INFO("Game started in level {}.", currentLevel);
SE_APP_WARN("Player tried to perform invalid action.");
SE_APP_ERROR("Game logic error: Invalid state detected.");
SE_APP_CRITICAL("Application crash: Fatal game data corruption.");
```

### Controlling Log Level and File Logging

You can control the verbosity of logs at runtime and enable/disable file output.

```cpp
// Set global log level to Info (e.g., for release builds that still need some info)
SilicaEngine::Logger::SetLogLevel(spdlog::level::info);

// Enable file logging (logs will be written to "MyGame.log" and rotated)
SilicaEngine::Logger::EnableFileLogging(true, "MyGame.log");

// Disable file logging
SilicaEngine::Logger::EnableFileLogging(false);
```

## Implementation Details

The `Logger` class internally manages two `spdlog::logger` instances, `s_EngineLogger` and `s_AppLogger`. It uses `spdlog::sinks::stdout_color_sink_mt` for console output and `spdlog::sinks::rotating_file_sink_mt` for file logging when enabled.

The `SE_DEBUG` preprocessor macro (defined by CMake in debug builds) controls the compilation of `TRACE` and `INFO` macros. When `SE_DEBUG` is not defined (e.g., in `Release` builds), these macros expand to `do {} while(0)`, effectively removing their code and performance impact from the final executable.