/**
 * @file SilicaEngine.h
 * @brief Main header for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.0.0
 * 
 * Include this file to access the complete engine API.
 */

#pragma once

// Version Information
#define SILICA_ENGINE_VERSION_MAJOR 1
#define SILICA_ENGINE_VERSION_MINOR 0
#define SILICA_ENGINE_VERSION_PATCH 0

// Platform Detection
#ifdef _WIN32
    #ifndef SILICA_PLATFORM_WINDOWS
        #define SILICA_PLATFORM_WINDOWS
    #endif
    #ifdef _WIN64
        #define SILICA_PLATFORM_WIN64
    #else
        #define SILICA_PLATFORM_WIN32
    #endif
#elif defined(__APPLE__)
    #ifndef SILICA_PLATFORM_MACOS
        #define SILICA_PLATFORM_MACOS
    #endif
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define SILICA_PLATFORM_IOS
    #endif
#elif defined(__linux__)
    #ifndef SILICA_PLATFORM_LINUX
        #define SILICA_PLATFORM_LINUX
    #endif
#else
    #error "Unsupported platform!"
#endif

// Core Engine Headers
#include "Core/Logger.h"
#include "Core/Application.h"
#include "Core/Window.h"

// Renderer Headers
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"

// Third-Party Headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace SilicaEngine {
    
    /// Initialize the engine
    bool Initialize();
    
    /// Shutdown and cleanup resources
    void Shutdown();
    
    /// Get version string in format "Major.Minor.Patch"
    const char* GetVersion();
    
    /// Check if engine is initialized
    bool IsInitialized();
}

// Convenience Macros
#define SE_VERSION_STRING "1.0.0"
#define SE_ENGINE_NAME "SilicaEngine"

// Debug Assertions
#ifdef SILICA_DEBUG
    #define SE_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                SE_CRITICAL("Assertion failed: {0}", message); \
                __debugbreak(); \
            } \
        } while(0)
#else
    #define SE_ASSERT(condition, message)
#endif