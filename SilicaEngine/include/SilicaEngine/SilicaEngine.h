/**
 * @file SilicaEngine.h
 * @brief Main header file for the SilicaEngine - High-Performance Game Engine
 * @version 1.0.0
 * 
 * This header provides access to all core functionality of the SilicaEngine.
 * Include this file to access the complete engine API.
 * 
 * Features:
 * - Modern OpenGL rendering with GLAD
 * - Cross-platform window management with GLFW
 * - Mathematics support with GLM
 * - High-performance logging with spdlog
 * - Modular architecture for easy extension
 */

#pragma once

// ============================================================================
// Version Information
// ============================================================================
#define SILICA_ENGINE_VERSION_MAJOR 1
#define SILICA_ENGINE_VERSION_MINOR 0
#define SILICA_ENGINE_VERSION_PATCH 0

// ============================================================================
// Platform Detection
// ============================================================================
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
    #define SILICA_PLATFORM_MACOS
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define SILICA_PLATFORM_IOS
    #endif
#elif defined(__linux__)
    #define SILICA_PLATFORM_LINUX
#else
    #error "Unsupported platform!"
#endif

// ============================================================================
// Core Engine Headers
// ============================================================================
#include "Core/Logger.h"
#include "Core/Application.h"
#include "Core/Window.h"

// ============================================================================
// Renderer Headers
// ============================================================================
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"

// ============================================================================
// Third-Party Headers (OpenGL, Math, etc.)
// ============================================================================
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ============================================================================
// Engine Namespace
// ============================================================================
namespace SilicaEngine {
    
    /**
     * @brief Initialize the SilicaEngine
     * @return true if initialization was successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Shutdown the SilicaEngine and cleanup resources
     */
    void Shutdown();
    
    /**
     * @brief Get the engine version string
     * @return Version string in format "Major.Minor.Patch"
     */
    const char* GetVersion();
    
    /**
     * @brief Check if the engine is initialized
     * @return true if initialized, false otherwise
     */
    bool IsInitialized();
}

// ============================================================================
// Convenience Macros
// ============================================================================
#define SE_VERSION_STRING "1.0.0"
#define SE_ENGINE_NAME "SilicaEngine"

// ============================================================================
// Assert Macros
// ============================================================================
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