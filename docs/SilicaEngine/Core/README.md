# SilicaEngine Core Modules

The Core modules form the foundation of SilicaEngine, providing essential services for application lifecycle management, input handling, resource management, error handling, and debugging. These modules are designed to be robust, efficient, and easy to use while providing comprehensive functionality for game development.

## Module Overview

### 🔧 **Application Module**
The central application framework that manages the engine lifecycle, windowing, and the main game loop. Provides a flexible structure for custom game logic through virtual override methods.

**Key Features:**
- Application lifecycle management (initialization, game loop, shutdown)
- Configurable window properties and OpenGL settings
- High-precision game loop with delta time calculation
- Event handling integration with windowing system
- Extensible design with virtual methods for custom logic

**Documentation:** [Application.md](Application.md)

### 🖥️ **Window Module**
Cross-platform window management and OpenGL context creation built on GLFW. Handles window properties, display modes, input events, and OpenGL setup.

**Key Features:**
- Cross-platform windowing (Windows, macOS, Linux)
- Configurable window properties (title, size, resizability, decoration)
- OpenGL context management with version control
- Input event integration with GLFW
- Display mode and fullscreen support

**Documentation:** [Window.md](Window.md)

### 🎮 **Input Module**
Comprehensive input management system that abstracts keyboard, mouse, and gamepad input into a unified, configurable interface with action-based bindings.

**Key Features:**
- Multi-device support (keyboard, mouse, gamepad)
- Action-based input system with rebinding support
- Input context layering for different game states
- Gamepad support with automatic device detection
- Configuration persistence and callback system

**Documentation:** [Input.md](Input.md)

### 📷 **Camera Module**
Flexible camera system supporting multiple projection types, movement modes, and input integration for both 3D and 2D applications.

**Key Features:**
- Multiple projection types (perspective, orthographic)
- Five movement modes (FirstPerson, ThirdPerson, Orbital, Free, Fixed)
- Integrated input processing with configurable sensitivity
- Smooth movement and constraint systems
- Screen-to-world ray casting for picking

**Documentation:** [Camera.md](Camera.md)

### 📝 **Logger Module**
High-performance logging system leveraging spdlog with dual loggers for engine and application messages, plus compile-time optimization.

**Key Features:**
- Dual-logger architecture (engine and application)
- High performance with spdlog backend
- Configurable output (console, file, rotating logs)
- Compile-time log level control for release builds
- Colored console output for easy readability

**Documentation:** [Logger.md](Logger.md)

### ⚠️ **Error Handling Modules**
Comprehensive error management system with categorized error codes and type-safe result templates for consistent error handling across the engine.

**Key Features:**
- Over 50 categorized error codes covering all subsystems
- Type-safe `Result<T>` and `ErrorResult<T>` templates
- Human-readable error messages and severity levels
- Implicit boolean conversion and default value fallbacks
- Error validation and conversion utilities

**Documentation:** [ErrorHandling.md](ErrorHandling.md)

### 🗂️ **Resource Management Modules**
Complete resource management solution with stable handles, asset management, dependency tracking, and debug resource leak detection.

**Key Features:**
- Stable resource handles that remain valid across reloads
- Asset management with reference counting and automatic cleanup
- Complex dependency tracking with circular dependency detection
- Hot reloading support for development workflows
- Debug resource leak detection for OpenGL objects

**Documentation:** [ResourceManagement.md](ResourceManagement.md)

### 📸 **Screenshot Module**
Comprehensive screenshot capture system with multiple format support, region capture, automatic timestamping, and batch operations.

**Key Features:**
- Multiple format support (PNG, JPEG, BMP, TGA, PPM, RAW)
- Configurable capture (region, format options, timestamping)
- Memory capture for post-processing
- Sequence capture for animation/timelapse
- Batch operations with ScreenshotManager

**Documentation:** [Screenshot.md](Screenshot.md)

## Architecture Overview

### Module Dependencies

```
Application
├── Window (GLFW integration)
├── Input (GLFW callbacks)
├── Camera (Input integration)
├── Logger (spdlog)
└── Screenshot (OpenGL framebuffer)

Resource Management
├── ResourceHandle (stable handles)
├── AssetManager (centralized loading)
├── AssetDependency (dependency tracking)
└── ResourceTracker (debug leak detection)

Error Handling
├── ErrorCodes (categorized errors)
└── Result (type-safe results)
```

### Design Principles

1. **Separation of Concerns**: Each module has a specific responsibility and minimal coupling
2. **Type Safety**: Extensive use of templates and strong typing for compile-time safety
3. **Error Handling**: Comprehensive error codes and result types for robust error management
4. **Performance**: Optimized for both development and production use
5. **Extensibility**: Virtual methods and callback systems for customization
6. **Thread Safety**: Clear documentation of thread safety guarantees

### Thread Safety Matrix

| Module | Thread Safety | Notes |
|--------|---------------|-------|
| Application | Single-threaded | Main application loop only |
| Window | Single-threaded | GLFW callbacks on main thread |
| Input | Thread-safe | Mutex protection for state access |
| Camera | Single-threaded | Designed for main thread use |
| Logger | Thread-safe | spdlog is thread-safe by design |
| Error Handling | Thread-safe | Read-only operations |
| Resource Management | Mixed | See individual module docs |
| Screenshot | Thread-safe | Mutex protection for shared state |

### Performance Characteristics

- **Memory Usage**: Efficient memory management with smart pointers and automatic cleanup
- **CPU Overhead**: Minimal overhead with compile-time optimizations
- **GPU Integration**: Direct OpenGL integration for maximum performance
- **I/O Operations**: Asynchronous operations where beneficial
- **Debug Features**: Zero overhead in release builds

## Getting Started

### Basic Application Setup

```cpp
#include <SilicaEngine/SilicaEngine.h>

class MyGame : public SilicaEngine::Application {
protected:
    void OnInitialize() override {
        // Initialize your game systems
    }
    
    void OnUpdate(float deltaTime) override {
        // Update game logic
    }
    
    void OnRender() override {
        // Render your game
    }
    
    void OnShutdown() override {
        // Cleanup resources
    }
};

int main() {
    MyGame game;
    return game.Run();
}
```

### Error Handling Example

```cpp
// Using Result template for simple operations
auto result = SilicaEngine::AssetManager::Initialize("assets/");
if (!result) {
    SE_ERROR("Failed to initialize asset manager: {}", result.errorMessage);
    return -1;
}

// Using ErrorResult for detailed error information
auto textureResult = SilicaEngine::AssetManager::LoadTexture("texture.png");
if (!textureResult) {
    auto errorCode = textureResult.errorCode;
    auto category = SilicaEngine::GetErrorCategory(errorCode);
    SE_ERROR("Texture loading failed: {} (Category: {})", 
             textureResult.errorMessage, 
             static_cast<int>(category));
}
```

### Resource Management Example

```cpp
// Load assets with automatic reference counting
auto texture = SilicaEngine::AssetManager::LoadTexture("player.png");
auto shader = SilicaEngine::AssetManager::LoadShader("basic", "basic.vert", "basic.frag");

// Use stable handles for long-term references
auto handle = SilicaEngine::ResourceRegistry::GetInstance().Register(texture.Get(), 
                                                                     SilicaEngine::ResourceType::Texture);

// Assets are automatically cleaned up when no longer referenced
texture.Reset(); // Release reference
```

## Best Practices

### Error Handling
- Always check return values from functions that can fail
- Use appropriate error result types for different scenarios
- Log errors with sufficient context for debugging
- Handle errors gracefully with fallback options

### Resource Management
- Use RAII principles with smart pointers and scope guards
- Register resources with the ResourceRegistry for stable handles
- Monitor memory usage and cleanup unused assets regularly
- Use dependency tracking for complex asset relationships

### Performance
- Minimize allocations in hot paths
- Use compile-time optimizations (debug vs release builds)
- Profile and optimize based on actual usage patterns
- Leverage async operations for I/O-bound tasks

### Thread Safety
- Follow the documented thread safety guarantees
- Use appropriate synchronization for shared resources
- Avoid cross-thread access to single-threaded modules
- Consider using async patterns for background operations

## Integration with Other Modules

The Core modules are designed to work seamlessly with other engine subsystems:

- **Renderer**: Provides window context and input for rendering
- **Debug**: Integrates with logging and error handling systems
- **Assets**: Uses resource management for asset loading and caching
- **Physics**: Receives input and camera data for physics simulation
- **Audio**: Uses resource management for audio file loading

## Future Enhancements

The Core modules are designed for extensibility and future enhancements:

- **Multi-threading**: Enhanced thread safety and async operations
- **Networking**: Integration with networking systems
- **Scripting**: Support for script-based configuration and behavior
- **Profiling**: Enhanced performance monitoring and profiling
- **Hot Reloading**: Runtime module reloading for development

The Core modules provide a solid foundation for building robust, performant, and maintainable games and applications with SilicaEngine. 