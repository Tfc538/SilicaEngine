# ============================================================================
# SilicaEngine - High-Performance Game Engine
# CMakeLists.txt
# Author: Tim Gatzke <post@tim-gatzke.de>
# ============================================================================

cmake_minimum_required(VERSION 3.16)

project(SilicaEngine
    VERSION 1.0.0
    DESCRIPTION "High-Performance OpenGL Game Engine"
    LANGUAGES CXX
)

# ============================================================================
# Library Sources
# ============================================================================

# Collect all source files
file(GLOB_RECURSE SILICA_SOURCES
    "src/*.cpp"
)

file(GLOB_RECURSE SILICA_HEADERS
    "include/*.h"
)

# Create the static library
add_library(SilicaEngine STATIC
    ${SILICA_SOURCES}
    ${SILICA_HEADERS}
)

# Add alias for consistency
add_library(SilicaEngine::SilicaEngine ALIAS SilicaEngine)

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(SilicaEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(SilicaEngine
    PUBLIC
        OpenGL::GL
        glfw
        glad_gl_core_43
        glm::glm
        spdlog::spdlog
)

# ============================================================================
# Compiler Features and Definitions
# ============================================================================
target_compile_features(SilicaEngine
    PUBLIC
        cxx_std_17
)

target_compile_definitions(SilicaEngine
    PUBLIC
        SILICA_ENGINE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        SILICA_ENGINE_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        SILICA_ENGINE_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    PRIVATE
        SILICA_ENGINE_EXPORTS
)

# ============================================================================
# Platform-specific settings
# ============================================================================
if(WIN32)
    target_compile_definitions(SilicaEngine
        PUBLIC
            SILICA_PLATFORM_WINDOWS
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )
elseif(APPLE)
    target_compile_definitions(SilicaEngine
        PUBLIC
            SILICA_PLATFORM_MACOS
    )
elseif(UNIX)
    target_compile_definitions(SilicaEngine
        PUBLIC
            SILICA_PLATFORM_LINUX
    )
endif()

# ============================================================================
# IDE Support
# ============================================================================
# Create source groups for IDEs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${SILICA_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SILICA_SOURCES})

# Set target properties
set_target_properties(SilicaEngine PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    FOLDER "Engine"
)

# ============================================================================
# Installation
# ============================================================================
include(GNUInstallDirs)

# Simplified installation without exports for now
install(TARGETS SilicaEngine
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# ============================================================================
# Summary
# ============================================================================
message(STATUS "SilicaEngine Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Sources: ${SILICA_SOURCES}")
message(STATUS "  Headers: ${SILICA_HEADERS}")