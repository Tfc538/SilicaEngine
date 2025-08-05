# SilicaEngine Documentation

Welcome to the SilicaEngine documentation! SilicaEngine is a modern, high-performance game engine built with C++ and OpenGL, designed for creating 2D and 3D applications with enterprise-grade features.

## Overview

SilicaEngine provides a robust foundation for game development with comprehensive error handling, resource management, debugging tools, and performance profiling capabilities. The engine is designed with modularity, thread safety, and performance in mind.

## Key Features

### Core Systems
- **Application Management**: Centralized application lifecycle management
- **Window Management**: Cross-platform window creation and management
- **Input System**: Multi-platform input handling with callback support
- **Camera System**: Flexible 2D and 3D camera controls
- **Resource Management**: Thread-safe resource handles and dependency tracking
- **Error Handling**: Comprehensive error codes and result-based error handling

### Rendering
- **OpenGL Renderer**: Modern OpenGL-based rendering pipeline
- **Batch Rendering**: Efficient batch rendering for 2D graphics
- **Shader System**: Flexible shader compilation and management
- **Texture Management**: Optimized texture loading and caching

### Debug & Development Tools
- **Debug Renderer**: Complete visualization API for debugging
- **Performance Profiler**: CPU/GPU timing and hierarchical profiling
- **Resource Tracker**: Memory leak detection and resource monitoring
- **Asset Dependencies**: Dependency graph with circular detection
- **Screenshot System**: High-quality screenshot capture

### Asset Management
- **Asset Manager**: Centralized asset loading and caching
- **Dependency Tracking**: Automatic dependency resolution
- **Circular Detection**: Prevents circular asset dependencies
- **Topological Sorting**: Optimized asset loading order

## Architecture

SilicaEngine follows a modular architecture with clear separation of concerns:

```
SilicaEngine/
├── Core/           # Core engine systems
├── Renderer/       # Graphics and rendering
├── Debug/          # Development and debugging tools
└── Fractura/       # Example application
```

## Getting Started

### Prerequisites
- C++17 compatible compiler
- CMake 3.15 or higher
- OpenGL 4.3 or higher
- GLFW3
- GLM 1.0.1

### Building

#### Windows
```batch
scripts\windows\build-debug.bat
```

#### Linux
```bash
scripts/linux/build-debug.sh
```

#### macOS
```bash
scripts/macos/build-debug.sh
```

### Running the Example

The engine includes Fractura, a demonstration application showcasing engine capabilities:

```bash
scripts/windows/run-fractura.bat  # Windows
scripts/linux/run-fractura.sh     # Linux
scripts/macos/run-fractura.sh     # macOS
```

## Documentation Structure

- **[Core Systems](SilicaEngine/Core/README.md)**: Application, Window, Input, Camera, and Resource Management
- **[Renderer](SilicaEngine/Renderer/README.md)**: Graphics pipeline and rendering systems
- **[Error Handling](SilicaEngine/Core/ErrorHandling.md)**: Error codes and exception handling
- **[Asset Management](SilicaEngine/Core/ResourceManagement.md)**: Resource loading and dependency tracking

## Development Status

SilicaEngine v1.1.0 is production-ready with the following completed features:

✅ **Completed Features**
- Comprehensive Error Codes System (50+ categorized errors)
- Resource Handle System (stable handles, thread-safe registry)
- Debug Rendering Module (complete visualization API)
- Asset Dependencies Tracking (dependency graph, circular detection)
- Performance Profiling System (CPU/GPU timing, memory tracking)
- OpenGL Error Checking (GL_CALL macro)
- Thread Safety (Input callbacks, Shader uniform cache)
- Static Initialization Order Fixes
- Performance Optimizations (AssetManager caching, async screenshots)
- Resource Leak Detection (ResourceTracker)
- Standardized Error Handling (Result<T> template)

🔄 **In Development**
- Event-based communication system
- Advanced rendering features
- Platform abstraction layer

## Contributing

Please refer to the project's contribution guidelines and ensure all code follows the established coding standards.

## License

[License information to be added]

---

For detailed API documentation, see the individual component documentation in the [SilicaEngine](SilicaEngine/) directory.

