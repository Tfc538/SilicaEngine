# 📜 CHANGELOG

## 1.0.0 - 2025-08-04  

### Codename: **Bedrock**

> The foundational release of SilicaEngine — built for performance, clarity, and modular growth.

---

### 🏗️ Core Architecture
- Modular static library structure
- Cross-platform support (Windows, macOS, Linux)
- Automatic platform detection and configuration
- Clean C++17 codebase with RAII principles

---

### 🪟 Window Management
- GLFW-based windowing layer
- Configurable window properties (title, size, transparency, MSAA, etc.)
- OpenGL 4.6 core context creation with full customization
- Window icon, position, and screenshot capture support
- VSync control and event polling
- Mouse and cursor mode control

---

### 🎨 Rendering System
- OpenGL renderer (GLAD-based)
- Primitive rendering: Triangles, Quads, Cubes, Lines, Grids
- Configurable render states: Depth Test, Culling, Blending, Wireframe
- Matrix stack: View, Projection, Model with full control
- GPU info: Vendor, renderer string, version queries
- OpenGL error tracking and logging

---

### 🧠 Shader System
- Full support for: Vertex, Fragment, Geometry, Compute, Tessellation shaders
- Creation from source and file
- Default shader system included
- Type-safe uniform setting (bools, ints, floats, vecs, mats, arrays)
- Uniform location caching for performance
- Introspection of active uniforms and attributes

---

### 🪵 Logging System
- Dual-logger setup: Engine + Application
- `spdlog` integration with colored output and file logging
- Multiple log levels (TRACE to CRITICAL)
- Easy macros: `SE_INFO`, `SE_ERROR`, etc.
- Dynamic log level configuration and flushing

---

### 🧱 Application Framework
- Custom `Application` class with overrideable lifecycle methods
- Delta time and FPS tracking
- Frame timing structure (`BeginFrame`, `EndFrame`)
- Configurable startup (OpenGL version, fullscreen, vsync, etc.)
- Input handling via callback-based event system

---

### 🛠 Debugging & Development
- Optional OpenGL debug context
- Real-time rendering statistics
- Assertion system for debug builds
- CMake-based build system with Visual Studio and macOS support
- Organized project structure with installation targets

---

### 🧪 Demo: Fractura
A simple testbed showcasing:
- Multiple animated 3D cubes
- Camera control (WASD + mouse)
- Wireframe toggle
- Color blending and dynamic transformation
- On-screen statistics and grid rendering

