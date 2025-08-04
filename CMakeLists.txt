# ============================================================================
# Caelis - Professional C++ Multi-Project Repository
# Root CMakeLists.txt
# Author: Tim Gatzke <post@tim-gatzke.de>
# ============================================================================

cmake_minimum_required(VERSION 3.16)

# ============================================================================
# Project Configuration
# ============================================================================
project(Caelis
    VERSION 1.0.0
    DESCRIPTION "C++ Game Engine with Example Game"
    LANGUAGES C CXX
)

# ============================================================================
# Global Settings
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ============================================================================
# Fix runtime library mismatch (LNK4098)
# ============================================================================
if(MSVC)
    # Ensures consistent use of /MD or /MDd (dynamic CRT) across all dependencies
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "" FORCE)
endif()

# ============================================================================
# Output directories
# ============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# Compiler-specific options
# ============================================================================
if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    # Suppress spdlog warnings about deprecated iterators
    add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

# ============================================================================
# Find Required Packages
# ============================================================================

# OpenGL
find_package(OpenGL REQUIRED)
if(NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found. Please install OpenGL development libraries.")
endif()

# GLFW
find_package(glfw3 CONFIG)
if(NOT glfw3_FOUND)
    message(STATUS "GLFW not found via find_package. Using FetchContent...")
    include(FetchContent)
    
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.9
    )
    
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(glfw)
endif()

# GLM
find_package(glm CONFIG)
if(NOT glm_FOUND)
    message(STATUS "GLM not found via find_package. Using FetchContent...")
    
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
    )
    
    set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(glm)
endif()

# spdlog
find_package(spdlog CONFIG)
if(NOT spdlog_FOUND)
    message(STATUS "spdlog not found via find_package. Using FetchContent...")
    
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
    )
    
    set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(spdlog)
endif()

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        73db193f853e2ee079bf3ca8a64aa2eaf6459043 #v2.0.8
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_43 STATIC REPRODUCIBLE LOADER API gl:core=4.3)

# STB (for image loading)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        f58f558c120e9b32c217290b80bad1a0729fbb2c
)
FetchContent_MakeAvailable(stb)

# Create interface library for STB
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

# ============================================================================
# Global Include Directories
# ============================================================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ============================================================================
# Subdirectories
# ============================================================================
add_subdirectory(SilicaEngine)
add_subdirectory(Fractura)

# ============================================================================
# Installation
# ============================================================================
install(TARGETS SilicaEngine
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY SilicaEngine/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# ============================================================================
# Summary
# ============================================================================
message(STATUS "")
message(STATUS "===========================================================================")
message(STATUS "  ${PROJECT_NAME} ${PROJECT_VERSION} Configuration Summary")
message(STATUS "===========================================================================")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  OpenGL: ${OPENGL_VERSION_STRING}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "===========================================================================")
message(STATUS "")
