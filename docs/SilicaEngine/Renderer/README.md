# SilicaEngine Renderer Modules

The Renderer modules provide a comprehensive OpenGL-based rendering system for SilicaEngine, offering high-level abstractions for shader management, texture handling, primitive rendering, and efficient batch rendering. These modules are designed to be performant, easy to use, and provide excellent debugging capabilities.

## Module Overview

### 🎨 **Renderer Module**
The main rendering system that provides high-level OpenGL abstractions, primitive drawing, state management, and performance statistics tracking.

**Key Features:**
- High-level OpenGL state management
- Primitive rendering (triangles, quads, cubes, lines, grids)
- Batch rendering for performance optimization
- Rendering statistics and performance monitoring
- OpenGL error checking and validation
- Matrix management (view, projection, model)

**Documentation:** [Renderer.md](Renderer.md)

### 🔧 **Shader Module**
OpenGL shader program management with compilation, linking, uniform management, and introspection capabilities.

**Key Features:**
- Shader compilation and linking from source strings or files
- Comprehensive uniform management with caching
- Support for all OpenGL shader types (vertex, fragment, geometry, compute, tessellation)
- Shader introspection and uniform location caching
- Thread-safe uniform operations
- Error handling with detailed compilation feedback

**Documentation:** [Shader.md](Shader.md)

### 🖼️ **Texture Module**
OpenGL texture wrapper with support for various formats, filtering modes, and efficient texture management.

**Key Features:**
- Texture loading from files and memory data
- Multiple texture formats and internal formats
- Configurable filtering and wrapping modes
- Mipmap generation and management
- Utility functions for common textures (white, black, checkerboard)
- Move semantics for efficient resource management

**Documentation:** [Texture.md](Texture.md)

### ⚡ **BatchRenderer Module**
High-performance instanced rendering system that reduces draw calls by batching similar objects.

**Key Features:**
- Instanced rendering for performance optimization
- Automatic batch management and flushing
- Configurable batch sizes for different use cases
- 60-80% performance improvement for scenes with many similar objects
- Efficient GPU resource management

**Documentation:** [BatchRenderer.md](BatchRenderer.md)

## Architecture Overview

### Module Dependencies

```
Renderer
├── Shader (shader program management)
├── Texture (texture binding and management)
├── BatchRenderer (instanced rendering)
└── DebugRenderer (debug visualization)

Shader
├── OpenGL (shader compilation/linking)
└── ErrorCodes (error handling)

Texture
├── OpenGL (texture creation/binding)
├── STB Image (file loading)
└── ErrorCodes (error handling)

BatchRenderer
├── Renderer (batch integration)
├── Shader (instanced shaders)
└── OpenGL (instanced rendering)
```

### Design Principles

1. **Performance First**: Optimized for high-performance rendering with minimal overhead
2. **Ease of Use**: High-level abstractions that hide OpenGL complexity
3. **Error Safety**: Comprehensive error checking and validation
4. **Resource Management**: Efficient OpenGL resource lifecycle management
5. **Debugging Support**: Extensive debugging capabilities and statistics
6. **Thread Safety**: Thread-safe operations where appropriate

### Performance Characteristics

- **Draw Call Optimization**: Batch rendering reduces draw calls by 60-80%
- **Memory Efficiency**: Smart resource management with move semantics
- **GPU Utilization**: Efficient use of GPU resources and memory bandwidth
- **CPU Overhead**: Minimal CPU overhead with optimized state management
- **Debug Features**: Zero overhead in release builds

## Getting Started

### Basic Rendering Setup

```cpp
#include <SilicaEngine/SilicaEngine.h>

// Initialize renderer
auto result = SilicaEngine::Renderer::Initialize();
if (!result) {
    SE_ERROR("Failed to initialize renderer: {}", result.errorMessage);
    return -1;
}

// Set up rendering state
SilicaEngine::Renderer::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
SilicaEngine::Renderer::SetDepthTest(true);
SilicaEngine::Renderer::SetBlending(true);

// In render loop
SilicaEngine::Renderer::BeginFrame();
SilicaEngine::Renderer::Clear();

// Draw primitives
SilicaEngine::Renderer::DrawQuad(glm::vec3(0, 0, 0), glm::vec2(1, 1), glm::vec4(1, 0, 0, 1));
SilicaEngine::Renderer::DrawCube(glm::vec3(2, 0, 0), glm::vec3(1, 1, 1), glm::vec4(0, 1, 0, 1));

SilicaEngine::Renderer::EndFrame();
```

### Shader Management

```cpp
// Create shader from files
auto shader = std::make_shared<SilicaEngine::Shader>();
auto result = shader->CreateFromFile("shaders/basic.vert", "shaders/basic.frag");
if (!result) {
    SE_ERROR("Shader creation failed: {}", result.errorMessage);
    return;
}

// Use shader
shader->Bind();
shader->SetMat4("viewProjection", viewProjectionMatrix);
shader->SetVec3("lightPosition", lightPos);
shader->SetFloat("ambientStrength", 0.1f);

// Draw with shader
SilicaEngine::Renderer::DrawQuad(position, size, color);
```

### Texture Management

```cpp
// Load texture from file
auto texture = SilicaEngine::Texture::Create("textures/player.png");
if (!texture || !texture->IsValid()) {
    SE_ERROR("Failed to load texture");
    return;
}

// Configure texture parameters
SilicaEngine::TextureParams params;
params.minFilter = SilicaEngine::TextureFilter::LinearMipmapLinear;
params.magFilter = SilicaEngine::TextureFilter::Linear;
params.wrapS = SilicaEngine::TextureWrap::ClampToEdge;
params.wrapT = SilicaEngine::TextureWrap::ClampToEdge;

// Create texture with custom parameters
auto customTexture = SilicaEngine::Texture::Create("textures/tile.png", params);

// Bind and use texture
texture->Bind(0);
SilicaEngine::Renderer::DrawTexturedQuad(position, size, texture, tint);
```

### Batch Rendering

```cpp
// Initialize batch renderer
SilicaEngine::BatchRenderer batchRenderer;
auto result = batchRenderer.Initialize();
if (!result) {
    SE_ERROR("Batch renderer initialization failed: {}", result.errorMessage);
    return;
}

// Add objects to batch
for (int i = 0; i < 1000; ++i) {
    glm::vec3 position(i * 2.0f, 0, 0);
    glm::vec3 size(1.0f, 1.0f, 1.0f);
    glm::vec4 color(1.0f, 0.5f, 0.2f, 1.0f);
    
    batchRenderer.AddCube(position, size, color);
    
    // Flush when batch is full
    if (batchRenderer.IsBatchFull()) {
        batchRenderer.FlushBatch();
    }
}

// Flush remaining objects
batchRenderer.FlushBatch();
```

## Advanced Usage

### Custom Shader Programs

```cpp
// Create shader from source strings
std::string vertexSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;
    
    uniform mat4 model;
    uniform mat4 viewProjection;
    
    out vec2 TexCoord;
    
    void main() {
        gl_Position = viewProjection * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)";

std::string fragmentSource = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec2 TexCoord;
    uniform sampler2D texture1;
    uniform vec3 tint;
    
    void main() {
        vec4 texColor = texture(texture1, TexCoord);
        FragColor = texColor * vec4(tint, 1.0);
    }
)";

auto shader = std::make_shared<SilicaEngine::Shader>();
shader->CreateFromString(vertexSource, fragmentSource);
```

### Texture Creation from Memory

```cpp
// Create texture from pixel data
int width = 256;
int height = 256;
std::vector<unsigned char> pixels(width * height * 4);

// Generate gradient pattern
for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
        int index = (y * width + x) * 4;
        pixels[index] = static_cast<unsigned char>((float)x / width * 255);     // R
        pixels[index + 1] = static_cast<unsigned char>((float)y / height * 255); // G
        pixels[index + 2] = 128;                                                  // B
        pixels[index + 3] = 255;                                                  // A
    }
}

auto texture = SilicaEngine::Texture::Create(width, height, 
    SilicaEngine::TextureFormat::RGBA, pixels.data());
```

### Performance Monitoring

```cpp
// Monitor rendering performance
SilicaEngine::Renderer::BeginFrame();

// ... rendering code ...

SilicaEngine::Renderer::EndFrame();

// Get performance statistics
const auto& stats = SilicaEngine::Renderer::GetStats();
SE_INFO("Draw calls: {}, Vertices: {}, Triangles: {}, Shader switches: {}, Texture binds: {}", 
        stats.drawCalls, stats.vertices, stats.triangles, 
        stats.shaderSwitches, stats.textureBinds);

// Reset statistics for next frame
SilicaEngine::Renderer::ResetStats();
```

### OpenGL Error Checking

```cpp
// Use GL_CALL macro for automatic error checking in debug builds
GL_CALL(glEnable(GL_DEPTH_TEST));
GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

// Manual error checking
if (!SilicaEngine::Renderer::CheckGLError("Custom operation")) {
    SE_ERROR("OpenGL error occurred during custom operation");
}
```

## Best Practices

### Performance Optimization
- Use batch rendering for similar objects
- Minimize shader switches and texture binds
- Use appropriate texture filtering and mipmaps
- Monitor rendering statistics and optimize bottlenecks
- Use instanced rendering for large numbers of similar objects

### Resource Management
- Use smart pointers for automatic resource cleanup
- Leverage move semantics for efficient texture transfers
- Monitor OpenGL resource usage and cleanup properly
- Use texture atlases to reduce texture bind operations

### Error Handling
- Always check return values from initialization functions
- Use GL_CALL macro for OpenGL operations in debug builds
- Validate shader compilation and linking
- Handle texture loading errors gracefully

### Debugging
- Enable debug rendering for visualization
- Monitor rendering statistics for performance analysis
- Use OpenGL debug contexts for detailed error information
- Validate shader uniforms and attributes

## Integration with Other Modules

The Renderer modules integrate seamlessly with other engine subsystems:

- **Core**: Uses window context and input for rendering
- **Debug**: Provides debug visualization and performance overlays
- **Assets**: Uses asset management for texture and shader loading
- **Camera**: Receives view and projection matrices for rendering
- **Physics**: Renders physics debug information and collision shapes

## Future Enhancements

The Renderer modules are designed for extensibility and future enhancements:

- **Vulkan Support**: Modern graphics API support
- **Compute Shaders**: GPU compute capabilities
- **Advanced Rendering**: PBR, post-processing, and advanced lighting
- **Multi-threading**: Enhanced multi-threaded rendering
- **VR/AR Support**: Virtual and augmented reality rendering

The Renderer modules provide a powerful, efficient, and easy-to-use rendering foundation for building high-performance graphics applications with SilicaEngine. 