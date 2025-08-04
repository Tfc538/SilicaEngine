# SilicaEngine Features

**Version:** 1.0.0 (Bedrock)
**Date:** 4. August 2025

SilicaEngine is a modern, cross-platform game engine built with C++17 and OpenGL. This document provides a comprehensive overview of all engine capabilities and features.

---

## 🏗️ Core Architecture

### Engine Foundation
- **Version Management**: Semantic versioning with major.minor.patch format
- **Cross-Platform Support**: Windows, macOS, and Linux compatibility
- **Modular Design**: Clean separation between core, rendering, and application layers
- **C++17 Standard**: Modern C++ features and best practices
- **Static Library Distribution**: Easy integration into projects

### Platform Detection
- Automatic platform detection and configuration
- Platform-specific optimizations and definitions
- Support for:
  - Windows (Win32/Win64)
  - macOS (including iOS detection)
  - Linux

---

## 🪟 Window Management

### Window Creation & Properties
- **GLFW-based Window System**: Cross-platform window creation
- **Configurable Window Properties**:
  - Title, dimensions (width/height)
  - Resizable, decorated, focused states
  - Maximized, visible, transparent options
  - MSAA (Multi-Sample Anti-Aliasing) support

### Window Operations
- Real-time window resizing
- Position management (get/set window position)
- Window state queries (focused, minimized, maximized)
- Window icon configuration
- Screenshot capture functionality
- User pointer management for callbacks

### Display & Context Management
- **OpenGL Context Setup**:
  - Configurable OpenGL version (4.6 default)
  - Core profile and forward compatibility
  - Debug context support
  - Custom color/depth/stencil buffer bits
- **VSync Control**: Enable/disable vertical synchronization
- **Cursor Management**: Normal, hidden, disabled cursor modes
- **Event Polling**: Window and input event processing

---

## 🎨 Rendering System

### Modern OpenGL Renderer
- **OpenGL 4.6 Core Profile**: Modern graphics pipeline
- **GLAD Function Loading**: Automatic OpenGL function pointer management
- **High-Performance Rendering**: Optimized draw calls and state management

### Rendering State Management
- **Depth Testing**: Enable/disable with configurable depth functions
- **Face Culling**: Back-face culling with configurable modes (front/back/both)
- **Alpha Blending**: Configurable blend functions and factors
- **Wireframe Mode**: Toggle between solid and wireframe rendering
- **Viewport Control**: Dynamic viewport sizing and positioning

### Primitive Rendering
- **Geometric Primitives**:
  - Triangles with custom vertices and colors
  - Quads with position, size, and color
  - Cubes with 3D positioning and scaling
  - Lines with configurable width and color
  - Grid rendering for debugging and visualization

### Matrix Management
- **3D Transformation Matrices**:
  - View matrix for camera positioning
  - Projection matrix for perspective/orthographic projection
  - Model matrix for object transformations
- **Matrix Stack Management**: Get and set transformation matrices

### Rendering Statistics
- **Performance Metrics**:
  - Draw call counting
  - Vertex and triangle statistics
  - Shader switch tracking
  - Texture bind monitoring
  - Frame-by-frame statistics reset

### GPU Information
- OpenGL version detection
- GPU vendor identification
- Renderer string retrieval
- Error checking and logging

---

## 🎯 Shader System

### Shader Management
- **Comprehensive Shader Support**:
  - Vertex shaders
  - Fragment shaders
  - Geometry shaders
  - Compute shaders
  - Tessellation control/evaluation shaders

### Shader Operations
- **Creation Methods**:
  - From source strings
  - From external files
  - Default shader creation
- **Compilation & Linking**: Automatic compilation with error checking
- **Binding Management**: Efficient shader program binding/unbinding

### Uniform Management
- **Type-Safe Uniform Setting**:
  - Boolean, integer, float values
  - Vector types (vec2, vec3, vec4)
  - Matrix types (mat3, mat4)
  - Array uniforms (integer and float arrays)
- **Performance Optimizations**:
  - Uniform location caching
  - Active uniform/attribute introspection
- **Error Handling**: Comprehensive compilation and linking error reporting

---

## 📊 Logging System

### Multi-Logger Architecture
- **Dual Logger Setup**:
  - Engine logger for internal messages
  - Application logger for game-specific logging
- **spdlog Integration**: High-performance logging library

### Logging Features
- **Multiple Log Levels**:
  - Trace: Detailed debug information
  - Debug: Development information
  - Info: General information messages
  - Warn: Warning messages
  - Error: Error conditions
  - Critical: Critical system errors

### Output Options
- **Colored Console Output**: Distinct color schemes for engine vs application
- **File Logging**: Optional file output with configurable filenames
- **Formatted Messages**: Template-based message formatting
- **Convenient Macros**: Easy-to-use logging macros (SE_INFO, SE_APP_ERROR, etc.)

### Logging Management
- Dynamic log level adjustment
- Logger initialization and shutdown
- Message flushing and resource cleanup

---

## 🎮 Application Framework

### Application Lifecycle
- **Complete Application Management**:
  - Initialization with custom configuration
  - Main game loop with timing
  - Graceful shutdown and cleanup
- **Configuration System**:
  - Window properties configuration
  - OpenGL version requirements
  - VSync and fullscreen options

### Game Loop & Timing
- **High-Precision Timing**:
  - Delta time calculation
  - FPS (Frames Per Second) tracking
  - Application runtime tracking
- **Frame Management**:
  - BeginFrame/EndFrame structure
  - Automatic timing updates

### Event Handling System
- **Comprehensive Input Events**:
  - Keyboard events (press, release, repeat)
  - Mouse button events
  - Mouse movement tracking
  - Mouse scroll wheel events
  - Window resize events

### Virtual Interface
- **Extensible Application Class**:
  - OnInitialize: Custom initialization logic
  - OnUpdate: Per-frame logic updates
  - OnRender: Custom rendering code
  - OnShutdown: Cleanup procedures
  - Event callbacks for input handling

---

## 🔧 Development & Debugging

### Debug Features
- **Error Checking**: Comprehensive OpenGL error detection and logging
- **Assert System**: Debug-mode assertions with breakpoint support
- **Performance Monitoring**: Real-time rendering statistics
- **Debug Context**: Optional OpenGL debug context creation

### Build System Integration
- **CMake Support**: Modern CMake configuration
- **IDE Integration**: Source file organization for Visual Studio, etc.
- **Installation System**: Proper library and header installation

---

## 📦 Dependencies & Integration

### Core Dependencies
- **OpenGL**: Modern graphics API (4.6 core profile)
- **GLFW**: Cross-platform window and input handling
- **GLAD**: OpenGL function loading
- **GLM**: Mathematics library for graphics
- **spdlog**: High-performance logging

### Integration Features
- Static library packaging
- Clean public API with namespacing
- Header-only configuration options
- Easy project integration

---

## 🚀 Performance Features

### Optimization Features
- **Efficient State Management**: Minimize OpenGL state changes
- **Batch Rendering**: Optimized geometry rendering
- **Memory Management**: RAII-based resource management
- **Fast Logging**: High-performance logging with minimal overhead

### Statistics & Profiling
- Real-time performance metrics
- Draw call optimization tracking
- Memory usage monitoring through proper resource cleanup
- Frame timing analysis

---

## 🎯 Example Applications

### Fractura Demo
The engine includes a comprehensive demo application showcasing:
- Multiple animated cubes with scaling and movement
- 3D camera controls (WASD movement, mouse look)
- Real-time statistics display
- Wireframe mode toggle
- Grid rendering for spatial reference
- Color-coded object rendering

---

## 🔮 Architectural Design

### Design Principles
- **Modern C++**: Leveraging C++17 features for clean, efficient code
- **RAII Resource Management**: Automatic resource cleanup
- **Static Interface**: High-performance static class design
- **Error Safety**: Comprehensive error checking and reporting
- **Cross-Platform**: Consistent behavior across all supported platforms

### Extension Points
- Shader system for custom rendering effects
- Application framework for game-specific logic
- Event system for custom input handling
- Modular architecture for adding new components

---

*This documentation reflects SilicaEngine v1.0.0. For the latest updates and API changes, please refer to the source code and inline documentation.*