/**
 * @file ErrorCodes.cpp
 * @brief Implementation of comprehensive error code system
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Core/ErrorCodes.h"

namespace SilicaEngine {

    const char* GetErrorMessage(EngineError error) {
        static const std::unordered_map<EngineError, const char*> errorMessages = {
            // Success
            {EngineError::Success, "Operation completed successfully"},
            
            // General errors
            {EngineError::Unknown, "Unknown error occurred"},
            {EngineError::NotImplemented, "Functionality not yet implemented"},
            {EngineError::InvalidParameter, "Invalid parameter provided"},
            {EngineError::NullPointer, "Null pointer encountered"},
            {EngineError::OutOfBounds, "Index or value out of bounds"},
            
            // Memory errors
            {EngineError::OutOfMemory, "Insufficient memory available"},
            {EngineError::MemoryCorruption, "Memory corruption detected"},
            {EngineError::AllocationFailed, "Memory allocation failed"},
            
            // Initialization errors
            {EngineError::InitializationFailed, "Initialization failed"},
            {EngineError::AlreadyInitialized, "Component already initialized"},
            {EngineError::NotInitialized, "Component not initialized"},
            {EngineError::DependencyMissing, "Required dependency missing"},
            
            // File/IO errors
            {EngineError::FileNotFound, "File not found"},
            {EngineError::FileAccessDenied, "File access denied"},
            {EngineError::FileCorrupted, "File is corrupted"},
            {EngineError::DirectoryNotFound, "Directory not found"},
            {EngineError::IOError, "Input/output error"},
            
            // Resource errors
            {EngineError::ResourceNotFound, "Resource not found"},
            {EngineError::ResourceCorrupted, "Resource is corrupted"},
            {EngineError::ResourceInUse, "Resource is currently in use"},
            {EngineError::ResourceLimitExceeded, "Resource limit exceeded"},
            {EngineError::InvalidResourceType, "Invalid resource type"},
            
            // OpenGL errors
            {EngineError::OpenGLError, "OpenGL error occurred"},
            {EngineError::OpenGLVersionUnsupported, "OpenGL version not supported"},
            {EngineError::ShaderCompilationFailed, "Shader compilation failed"},
            {EngineError::ShaderLinkingFailed, "Shader linking failed"},
            {EngineError::TextureLoadFailed, "Texture loading failed"},
            {EngineError::FramebufferIncomplete, "Framebuffer is incomplete"},
            
            // Input system errors
            {EngineError::InputDeviceNotFound, "Input device not found"},
            {EngineError::InputBindingInvalid, "Input binding is invalid"},
            {EngineError::InputContextInvalid, "Input context is invalid"},
            {EngineError::GamepadNotConnected, "Gamepad not connected"},
            
            // Window/Display errors
            {EngineError::WindowCreationFailed, "Window creation failed"},
            {EngineError::DisplayModeUnsupported, "Display mode not supported"},
            {EngineError::FullscreenFailed, "Fullscreen mode failed"},
            {EngineError::SwapChainError, "Swap chain error"},
            
            // Asset loading errors
            {EngineError::AssetNotFound, "Asset not found"},
            {EngineError::AssetCorrupted, "Asset is corrupted"},
            {EngineError::AssetVersionMismatch, "Asset version mismatch"},
            {EngineError::AssetDependencyMissing, "Asset dependency missing"},
            {EngineError::AssetLoadTimeout, "Asset loading timed out"},
            
            // Camera system errors
            {EngineError::CameraInvalidMode, "Invalid camera mode"},
            {EngineError::CameraConstraintViolation, "Camera constraint violated"},
            {EngineError::CameraProjectionInvalid, "Invalid camera projection"},
            
            // Screenshot errors
            {EngineError::ScreenshotCaptureFailed, "Screenshot capture failed"},
            {EngineError::ScreenshotEncodingFailed, "Screenshot encoding failed"},
            {EngineError::ScreenshotFormatUnsupported, "Screenshot format not supported"},
            {EngineError::ScreenshotPermissionDenied, "Screenshot permission denied"}
        };
        
        auto it = errorMessages.find(error);
        return it != errorMessages.end() ? it->second : "Unknown error code";
    }
    
    ErrorCategory GetErrorCategory(EngineError error) {
        int errorValue = static_cast<int>(error);
        
        if (errorValue == 0) return ErrorCategory::General;  // Success
        if (errorValue >= 1 && errorValue <= 99) return ErrorCategory::General;
        if (errorValue >= 100 && errorValue <= 199) return ErrorCategory::Memory;
        if (errorValue >= 200 && errorValue <= 299) return ErrorCategory::Initialization;
        if (errorValue >= 300 && errorValue <= 399) return ErrorCategory::FileIO;
        if (errorValue >= 400 && errorValue <= 499) return ErrorCategory::Resource;
        if (errorValue >= 500 && errorValue <= 599) return ErrorCategory::Graphics;
        if (errorValue >= 600 && errorValue <= 699) return ErrorCategory::Input;
        if (errorValue >= 700 && errorValue <= 799) return ErrorCategory::Window;
        if (errorValue >= 800 && errorValue <= 899) return ErrorCategory::Asset;
        if (errorValue >= 900 && errorValue <= 999) return ErrorCategory::Camera;
        if (errorValue >= 1000 && errorValue <= 1099) return ErrorCategory::Screenshot;
        
        return ErrorCategory::General;
    }
    
    EngineError FromInt(int value) {
        // Validate that the integer corresponds to a valid error code
        static const std::unordered_map<int, EngineError> validCodes = {
            {0, EngineError::Success},
            {1, EngineError::Unknown},
            {2, EngineError::NotImplemented},
            {3, EngineError::InvalidParameter},
            {4, EngineError::NullPointer},
            {5, EngineError::OutOfBounds},
            {100, EngineError::OutOfMemory},
            {101, EngineError::MemoryCorruption},
            {102, EngineError::AllocationFailed},
            {200, EngineError::InitializationFailed},
            {201, EngineError::AlreadyInitialized},
            {202, EngineError::NotInitialized},
            {203, EngineError::DependencyMissing},
            {300, EngineError::FileNotFound},
            {301, EngineError::FileAccessDenied},
            {302, EngineError::FileCorrupted},
            {303, EngineError::DirectoryNotFound},
            {304, EngineError::IOError},
            {400, EngineError::ResourceNotFound},
            {401, EngineError::ResourceCorrupted},
            {402, EngineError::ResourceInUse},
            {403, EngineError::ResourceLimitExceeded},
            {404, EngineError::InvalidResourceType},
            {500, EngineError::OpenGLError},
            {501, EngineError::OpenGLVersionUnsupported},
            {502, EngineError::ShaderCompilationFailed},
            {503, EngineError::ShaderLinkingFailed},
            {504, EngineError::TextureLoadFailed},
            {505, EngineError::FramebufferIncomplete},
            {600, EngineError::InputDeviceNotFound},
            {601, EngineError::InputBindingInvalid},
            {602, EngineError::InputContextInvalid},
            {603, EngineError::GamepadNotConnected},
            {700, EngineError::WindowCreationFailed},
            {701, EngineError::DisplayModeUnsupported},
            {702, EngineError::FullscreenFailed},
            {703, EngineError::SwapChainError},
            {800, EngineError::AssetNotFound},
            {801, EngineError::AssetCorrupted},
            {802, EngineError::AssetVersionMismatch},
            {803, EngineError::AssetDependencyMissing},
            {804, EngineError::AssetLoadTimeout},
            {900, EngineError::CameraInvalidMode},
            {901, EngineError::CameraConstraintViolation},
            {902, EngineError::CameraProjectionInvalid},
            {1000, EngineError::ScreenshotCaptureFailed},
            {1001, EngineError::ScreenshotEncodingFailed},
            {1002, EngineError::ScreenshotFormatUnsupported},
            {1003, EngineError::ScreenshotPermissionDenied}
        };
        
        auto it = validCodes.find(value);
        return it != validCodes.end() ? it->second : EngineError::Unknown;
    }
    
    int GetErrorSeverity(EngineError error) {
        switch (error) {
            case EngineError::Success:
                return 0;  // Success
                
            case EngineError::NotImplemented:
            case EngineError::AssetVersionMismatch:
                return 1;  // Warning
                
            case EngineError::OutOfMemory:
            case EngineError::MemoryCorruption:
            case EngineError::InitializationFailed:
            case EngineError::OpenGLVersionUnsupported:
            case EngineError::ShaderCompilationFailed:
            case EngineError::ShaderLinkingFailed:
                return 3;  // Critical
                
            default:
                return 2;  // Error
        }
    }

} // namespace SilicaEngine