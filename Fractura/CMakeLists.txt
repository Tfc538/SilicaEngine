# ============================================================================
# Fractura - Example Game using SilicaEngine
# CMakeLists.txt
# Author: Tim Gatzke <post@tim-gatzke.de>
# ============================================================================

cmake_minimum_required(VERSION 3.16)

project(Fractura
    VERSION 1.0.0
    DESCRIPTION "Example Game demonstrating SilicaEngine capabilities"
    LANGUAGES CXX
)

# ============================================================================
# Executable Sources
# ============================================================================

# Collect all source files
file(GLOB_RECURSE FRACTURA_SOURCES
    "src/*.cpp"
    "src/*.h"
)

# Create the executable
add_executable(Fractura
    ${FRACTURA_SOURCES}
)

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(Fractura
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(Fractura
    PRIVATE
        SilicaEngine::SilicaEngine
)

# ============================================================================
# Compiler Features
# ============================================================================
target_compile_features(Fractura
    PRIVATE
        cxx_std_17
)

# ============================================================================
# Platform-specific settings
# ============================================================================
if(WIN32)
    target_compile_definitions(Fractura
        PRIVATE
            FRACTURA_PLATFORM_WINDOWS
    )
    
    # Set as console application on Windows
    set_target_properties(Fractura PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
elseif(APPLE)
    target_compile_definitions(Fractura
        PRIVATE
            FRACTURA_PLATFORM_MACOS
    )
elseif(UNIX)
    target_compile_definitions(Fractura
        PRIVATE
            FRACTURA_PLATFORM_LINUX
    )
endif()

# ============================================================================
# Asset Management
# ============================================================================

# Copy assets to build directory
add_custom_command(TARGET Fractura POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Fractura>/assets
    COMMENT "Copying assets to build directory"
)

# ============================================================================
# IDE Support
# ============================================================================

# Create source groups for IDEs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${FRACTURA_SOURCES})

# Set target properties
set_target_properties(Fractura PROPERTIES
    VERSION ${PROJECT_VERSION}
    FOLDER "Game"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ============================================================================
# Installation
# ============================================================================
install(TARGETS Fractura
    RUNTIME DESTINATION bin
)

install(DIRECTORY assets/
    DESTINATION bin/assets
    FILES_MATCHING 
    PATTERN "*.glsl"
    PATTERN "*.txt"
    PATTERN "*.png"
    PATTERN "*.jpg"
    PATTERN "*.obj"
    PATTERN "*.mtl"
)

# ============================================================================
# Summary
# ============================================================================
message(STATUS "Fractura Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Sources: ${FRACTURA_SOURCES}")
message(STATUS "  Assets will be copied to build directory")