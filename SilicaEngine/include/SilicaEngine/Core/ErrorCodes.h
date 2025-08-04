/**
 * @file ErrorCodes.h
 * @brief Comprehensive error code system for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Standardized error codes for consistent error handling across the engine.
 */

#pragma once

#include <string>
#include <unordered_map>

namespace SilicaEngine {

    /// Comprehensive error codes for all engine operations
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

    /// Error category for grouping related errors
    enum class ErrorCategory {
        General,
        Memory,
        Initialization,
        FileIO,
        Resource,
        Graphics,
        Input,
        Window,
        Asset,
        Camera,
        Screenshot
    };

    /// Get human-readable error message from error code
    const char* GetErrorMessage(EngineError error);
    
    /// Get error category from error code
    ErrorCategory GetErrorCategory(EngineError error);
    
    /// Check if error code represents success
    inline bool IsSuccess(EngineError error) {
        return error == EngineError::Success;
    }
    
    /// Check if error code represents a failure
    inline bool IsFailure(EngineError error) {
        return error != EngineError::Success;
    }
    
    /// Convert error code to integer value
    inline int ToInt(EngineError error) {
        return static_cast<int>(error);
    }
    
    /// Convert integer to error code (with validation)
    EngineError FromInt(int value);
    
    /// Get the severity level of an error (0=success, 1=warning, 2=error, 3=critical)
    int GetErrorSeverity(EngineError error);

    /// Error result template that includes error codes
    template<typename T>
    struct ErrorResult {
        EngineError errorCode = EngineError::Success;
        std::string errorMessage;
        T value = {};
        
        static ErrorResult Success(const T& val) {
            ErrorResult result;
            result.errorCode = EngineError::Success;
            result.value = val;
            return result;
        }
        
        static ErrorResult Error(EngineError code, const std::string& message = "") {
            ErrorResult result;
            result.errorCode = code;
            result.errorMessage = message.empty() ? GetErrorMessage(code) : message;
            return result;
        }
        
        // Implicit conversion to bool for easy checking
        operator bool() const { 
            return IsSuccess(errorCode); 
        }
        
        // Check if this is a specific error
        bool IsError(EngineError code) const {
            return errorCode == code;
        }
        
        // Get value or default
        T GetValueOr(const T& defaultValue) const {
            return IsSuccess(errorCode) ? value : defaultValue;
        }
    };
    
    /// Specialized version for void operations
    template<>
    struct ErrorResult<void> {
        EngineError errorCode = EngineError::Success;
        std::string errorMessage;
        
        static ErrorResult Success() {
            ErrorResult result;
            result.errorCode = EngineError::Success;
            return result;
        }
        
        static ErrorResult Error(EngineError code, const std::string& message = "") {
            ErrorResult result;
            result.errorCode = code;
            result.errorMessage = message.empty() ? GetErrorMessage(code) : message;
            return result;
        }
        
        // Implicit conversion to bool for easy checking
        operator bool() const { 
            return IsSuccess(errorCode); 
        }
        
        // Check if this is a specific error
        bool IsError(EngineError code) const {
            return errorCode == code;
        }
    };
    
    /// Common error result types
    using VoidErrorResult = ErrorResult<void>;
    using BoolErrorResult = ErrorResult<bool>;
    using StringErrorResult = ErrorResult<std::string>;
    using IntErrorResult = ErrorResult<int>;
    using FloatErrorResult = ErrorResult<float>;

} // namespace SilicaEngine