# Error Handling Modules

The error handling system in SilicaEngine provides a comprehensive, standardized approach to error management across the entire engine. It consists of two main components: the `ErrorCodes` system for categorized error identification and the `Result` template for type-safe error return values.

## Key Features

*   **Comprehensive Error Codes**: Over 50 categorized error codes covering all engine subsystems.
*   **Categorized Error System**: Errors are organized into logical categories (General, Memory, Graphics, etc.) for easier debugging and handling.
*   **Type-Safe Error Results**: Template-based `Result<T>` and `ErrorResult<T>` types for safe error handling.
*   **Human-Readable Messages**: Built-in error message strings for all error codes.
*   **Error Severity Levels**: Four-level severity system (Success, Warning, Error, Critical) for error prioritization.
*   **Implicit Boolean Conversion**: Error results can be used directly in conditional statements.
*   **Default Value Fallbacks**: Safe value extraction with fallback defaults.

## Core Components

### EngineError Enum

The `EngineError` enum defines all possible error codes in the engine, organized by category:

```cpp
enum class EngineError {
    // Success
    Success = 0,
    
    // General errors (1-99)
    Unknown = 1,
    NotImplemented = 2,
    InvalidParameter = 3,
    NullPointer = 4,
    OutOfBounds = 5,
    
    // Memory errors (100-199)
    OutOfMemory = 100,
    MemoryCorruption = 101,
    AllocationFailed = 102,
    
    // Initialization errors (200-299)
    InitializationFailed = 200,
    AlreadyInitialized = 201,
    NotInitialized = 202,
    DependencyMissing = 203,
    
    // File/IO errors (300-399)
    FileNotFound = 300,
    FileAccessDenied = 301,
    FileCorrupted = 302,
    DirectoryNotFound = 303,
    IOError = 304,
    
    // Resource errors (400-499)
    ResourceNotFound = 400,
    ResourceCorrupted = 401,
    ResourceInUse = 402,
    ResourceLimitExceeded = 403,
    InvalidResourceType = 404,
    
    // OpenGL errors (500-599)
    OpenGLError = 500,
    OpenGLVersionUnsupported = 501,
    ShaderCompilationFailed = 502,
    ShaderLinkingFailed = 503,
    TextureLoadFailed = 504,
    FramebufferIncomplete = 505,
    
    // Input system errors (600-699)
    InputDeviceNotFound = 600,
    InputBindingInvalid = 601,
    InputContextInvalid = 602,
    GamepadNotConnected = 603,
    
    // Window/Display errors (700-799)
    WindowCreationFailed = 700,
    DisplayModeUnsupported = 701,
    FullscreenFailed = 702,
    SwapChainError = 703,
    
    // Asset loading errors (800-899)
    AssetNotFound = 800,
    AssetCorrupted = 801,
    AssetVersionMismatch = 802,
    AssetDependencyMissing = 803,
    AssetLoadTimeout = 804,
    
    // Camera system errors (900-999)
    CameraInvalidMode = 900,
    CameraConstraintViolation = 901,
    CameraProjectionInvalid = 902,
    
    // Screenshot errors (1000-1099)
    ScreenshotCaptureFailed = 1000,
    ScreenshotEncodingFailed = 1001,
    ScreenshotFormatUnsupported = 1002,
    ScreenshotPermissionDenied = 1003
};
```

### ErrorCategory Enum

Errors are categorized for easier handling and debugging:

```cpp
enum class ErrorCategory {
    General,        // General errors (1-99)
    Memory,         // Memory-related errors (100-199)
    Initialization, // Initialization errors (200-299)
    FileIO,         // File/IO errors (300-399)
    Resource,       // Resource errors (400-499)
    Graphics,       // OpenGL/Graphics errors (500-599)
    Input,          // Input system errors (600-699)
    Window,         // Window/Display errors (700-799)
    Asset,          // Asset loading errors (800-899)
    Camera,         // Camera system errors (900-999)
    Screenshot      // Screenshot errors (1000-1099)
};
```

## ErrorResult Template

The `ErrorResult<T>` template provides a type-safe way to return values with error information:

```cpp
template<typename T>
struct ErrorResult {
    EngineError errorCode = EngineError::Success;
    std::string errorMessage;
    T value = {};
    
    static ErrorResult Success(const T& val);
    static ErrorResult Error(EngineError code, const std::string& message = "");
    
    operator bool() const;  // Implicit conversion to bool
    bool IsError(EngineError code) const;
    T GetValueOr(const T& defaultValue) const;
};
```

### Specialized ErrorResult Types

Common error result types are predefined:

```cpp
using VoidErrorResult = ErrorResult<void>;
using BoolErrorResult = ErrorResult<bool>;
using StringErrorResult = ErrorResult<std::string>;
using IntErrorResult = ErrorResult<int>;
using FloatErrorResult = ErrorResult<float>;
```

## Result Template

The `Result<T>` template provides a simpler alternative to `ErrorResult<T>` for cases where detailed error codes aren't needed:

```cpp
template<typename T>
struct Result {
    bool success = false;
    std::string errorMessage;
    std::optional<T> value;
    
    static Result Success(const T& val);
    static Result Error(const std::string& message);
    
    operator bool() const;  // Implicit conversion to bool
    T GetValueOr(const T& defaultValue) const;
};
```

### Common Result Types

```cpp
using BoolResult = Result<bool>;
using StringResult = Result<std::string>;
using IntResult = Result<int>;
```

## Public Interface

### Error Code Functions

```cpp
const char* GetErrorMessage(EngineError error);
ErrorCategory GetErrorCategory(EngineError error);
bool IsSuccess(EngineError error);
bool IsFailure(EngineError error);
int ToInt(EngineError error);
EngineError FromInt(int value);
int GetErrorSeverity(EngineError error);
```

*   **`GetErrorMessage(EngineError error)`**: Returns a human-readable error message for the given error code.
*   **`GetErrorCategory(EngineError error)`**: Returns the category of the error code.
*   **`IsSuccess(EngineError error)`**: Returns `true` if the error code represents success.
*   **`IsFailure(EngineError error)`**: Returns `true` if the error code represents a failure.
*   **`ToInt(EngineError error)`**: Converts an error code to its integer value.
*   **`FromInt(int value)`**: Converts an integer to an error code (with validation).
*   **`GetErrorSeverity(EngineError error)`**: Returns the severity level (0=success, 1=warning, 2=error, 3=critical).

### ErrorResult Static Methods

```cpp
template<typename T>
static ErrorResult<T> ErrorResult<T>::Success(const T& val);
static ErrorResult<T> ErrorResult<T>::Error(EngineError code, const std::string& message = "");
```

*   **`Success(const T& val)`**: Creates a successful result with the given value.
*   **`Error(EngineError code, const std::string& message)`**: Creates an error result with the specified error code and optional custom message.

### Result Static Methods

```cpp
template<typename T>
static Result<T> Result<T>::Success(const T& val);
static Result<T> Result<T>::Error(const std::string& message);
```

*   **`Success(const T& val)`**: Creates a successful result with the given value.
*   **`Error(const std::string& message)`**: Creates an error result with the specified error message.

## Usage Examples

### Basic Error Handling

```cpp
// Function that can fail
ErrorResult<std::string> LoadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return ErrorResult<std::string>::Error(
            EngineError::FileNotFound, 
            "Could not open file: " + path
        );
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return ErrorResult<std::string>::Success(content);
}

// Using the function
auto result = LoadFile("config.txt");
if (result) {  // Implicit bool conversion
    std::cout << "File loaded: " << result.value << std::endl;
} else {
    std::cout << "Error: " << result.errorMessage << std::endl;
    std::cout << "Error code: " << ToInt(result.errorCode) << std::endl;
}
```

### Error Categories and Severity

```cpp
auto result = LoadFile("config.txt");
if (!result) {
    auto category = GetErrorCategory(result.errorCode);
    auto severity = GetErrorSeverity(result.errorCode);
    
    switch (category) {
        case ErrorCategory::FileIO:
            // Handle file-related errors
            break;
        case ErrorCategory::Resource:
            // Handle resource-related errors
            break;
        default:
            // Handle other errors
            break;
    }
    
    if (severity >= 3) {  // Critical error
        // Log critical error and potentially exit
        SE_CRITICAL("Critical error: {}", result.errorMessage);
    }
}
```

### Using Result Template

```cpp
// Simpler error handling with Result
Result<int> ParseNumber(const std::string& str) {
    try {
        int value = std::stoi(str);
        return Result<int>::Success(value);
    } catch (const std::exception& e) {
        return Result<int>::Error("Failed to parse number: " + str);
    }
}

// Using the function
auto result = ParseNumber("123");
if (result) {
    int value = result.value.value();  // std::optional access
    std::cout << "Parsed: " << value << std::endl;
} else {
    std::cout << "Error: " << result.errorMessage << std::endl;
}
```

### Default Value Fallbacks

```cpp
// Get value with fallback
auto result = LoadFile("config.txt");
std::string content = result.GetValueOr("default content");

// For Result template
auto parseResult = ParseNumber("invalid");
int value = parseResult.GetValueOr(0);  // Default to 0 if parsing fails
```

### Error Code Validation

```cpp
// Convert integer to error code safely
int errorValue = 500;
EngineError error = FromInt(errorValue);
if (error == EngineError::OpenGLError) {
    // Handle OpenGL error
}

// Check specific errors
auto result = LoadFile("config.txt");
if (result.IsError(EngineError::FileNotFound)) {
    // Handle file not found specifically
} else if (result.IsError(EngineError::FileAccessDenied)) {
    // Handle permission denied
}
```

### Void Operations

```cpp
// For operations that don't return a value
ErrorResult<void> InitializeSystem() {
    if (!InitializeSubsystem()) {
        return ErrorResult<void>::Error(
            EngineError::InitializationFailed,
            "Failed to initialize subsystem"
        );
    }
    return ErrorResult<void>::Success();
}

// Using void result
auto result = InitializeSystem();
if (!result) {
    SE_ERROR("Initialization failed: {}", result.errorMessage);
}
```

## Implementation Details

The error handling system uses a hash map for efficient error message lookup and provides validation for integer-to-error-code conversion. Error codes are organized in ranges to make category determination efficient.

The `ErrorResult<T>` template provides more detailed error information than the simpler `Result<T>` template, making it suitable for engine internals where specific error codes are needed for debugging and error recovery.

Both templates support implicit boolean conversion, making them easy to use in conditional statements. The `GetValueOr()` method provides safe value extraction with fallback defaults, preventing crashes from accessing invalid values.

## Error Severity Levels

The system defines four severity levels:

*   **0 - Success**: Operation completed successfully
*   **1 - Warning**: Non-critical issue that doesn't prevent operation
*   **2 - Error**: Standard error that prevents normal operation
*   **3 - Critical**: Severe error that may require application shutdown

Critical errors include memory corruption, initialization failures, and unsupported OpenGL versions, while warnings include unimplemented features and asset version mismatches. 