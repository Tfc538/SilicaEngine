# Window Module

The `Window` module in SilicaEngine provides a cross-platform abstraction for window management and OpenGL context creation, primarily built upon the GLFW library. It handles window properties, display modes, input events, and OpenGL setup, providing a flexible and robust environment for rendering.

## Key Features

*   **Cross-Platform Windowing**: Utilizes GLFW to create and manage windows across Windows, macOS, and Linux.
*   **Configurable Window Properties**: Allows detailed customization of window title, dimensions, resizability, decoration, focus, visibility, transparency, and MSAA (Multi-Sample Anti-Aliasing) levels.
*   **OpenGL Context Management**: Sets up an OpenGL rendering context with configurable major/minor versions, core/compatibility profiles, and optional debug context for development.
*   **Input Event Handling**: Integrates with GLFW's event system to poll for and process keyboard, mouse button, cursor position, and scroll events.
*   **Buffer Swapping**: Manages front and back buffer swapping for smooth rendering.
*   **Screenshot Capability**: Provides basic functionality to capture the current framebuffer content to a file.
*   **VSync Control**: Toggles vertical synchronization to control frame rate.

## WindowProperties

This struct defines the visual and behavioral properties of the window.

```cpp
struct WindowProperties {
    std::string title = "SilicaEngine Window";
    uint32_t width = 1280;
    uint32_t height = 720;
    bool resizable = true;
    bool decorated = true;
    bool focused = true;
    bool maximized = false;
    bool visible = true;
    bool transparent = false;
    int samples = 0; // MSAA samples (0 = disabled)
};
```

## OpenGLProperties

This struct defines the desired characteristics of the OpenGL rendering context.

```cpp
struct OpenGLProperties {
    int majorVersion = 4;
    int minorVersion = 6;
    bool coreProfile = true;
    bool forwardCompatible = true;
    bool debugContext = false; // Note: Conditionally enabled by SE_DEBUG in Application.cpp
    int colorBits = 24;
    int depthBits = 24;
    int stencilBits = 8;
};
```

## Public Interface

### Constructor and Destructor

```cpp
Window(const WindowProperties& windowProps = WindowProperties{},
       const OpenGLProperties& openglProps = OpenGLProperties{});
~Window();
```
*   **`Window(...)`**: Constructs a Window object with specified properties. Default properties are used if not provided.
*   **`~Window()`**: Destroys the GLFW window and cleans up associated resources.

### Core Lifecycle Methods

```cpp
ErrorResult<void> Initialize();
ErrorResult<void> Shutdown();
void PollEvents();
void SwapBuffers();
bool ShouldClose() const;
void SetShouldClose(bool shouldClose);
void MakeContextCurrent();
GLFWwindow* GetNativeWindow() const;
```
*   **`Initialize()`**: Initializes GLFW, creates the window, and sets up the OpenGL context. Returns `ErrorResult<void>` to indicate success or failure.
*   **`Shutdown()`**: Destroys the window, terminates GLFW, and cleans up resources.
*   **`PollEvents()`**: Processes all pending window events (e.g., input, resize). Should be called once per frame.
*   **`SwapBuffers()`**: Swaps the front and back buffers, displaying the rendered frame.
*   **`ShouldClose()`**: Returns `true` if the window close flag has been set (e.g., by user clicking close button).
*   **`SetShouldClose(bool shouldClose)`**: Manually sets the window's close flag.
*   **`MakeContextCurrent()`**: Makes the window's OpenGL context current on the calling thread.
*   **`GetNativeWindow()`**: Returns the raw `GLFWwindow*` handle, useful for direct GLFW API interaction.

### Property and State Management

```cpp
uint32_t GetWidth() const;
uint32_t GetHeight() const;
void GetSize(uint32_t& width, uint32_t& height) const;
void SetSize(uint32_t width, uint32_t height);
// ... similar methods for position, title, focus, minimize, maximize, icon, VSync, cursor mode, cursor position
```
These methods allow querying and modifying various window properties and states at runtime.

### Utilities

```cpp
void SetUserPointer(void* pointer);
void* GetUserPointer() const;
bool IsInitialized() const;
const WindowProperties& GetWindowProperties() const;
const OpenGLProperties& GetOpenGLProperties() const;
bool SaveScreenshot(const std::string& filename) const;
```
*   **`SetUserPointer()` / `GetUserPointer()`**: Allows associating a custom pointer with the window, commonly used for passing `this` pointers to static GLFW callbacks.
*   **`IsInitialized()`**: Checks if the window has been successfully initialized.
*   **`GetWindowProperties()` / `GetOpenGLProperties()`**: Returns the configuration structs used during initialization.
*   **`SaveScreenshot(const std::string& filename)`**: Captures the current framebuffer and saves it to a PPM file. (Note: Current implementation uses basic PPM format, `Screenshot` module provides more robust options.)

## Implementation Details

The `Window` class uses GLFW for all underlying window and context operations. `Initialize()` configures GLFW hints based on `OpenGLProperties` and `WindowProperties` before creating the actual `GLFWwindow` object. OpenGL function pointers are loaded using GLAD. Default OpenGL states (depth test, culling, blending, clear color) are set up in `SetupOpenGLDefaults()`.

Crucially, `SaveScreenshot` currently uses a simple PPM format, which is primarily for demonstration. The dedicated `Screenshot` module in `SilicaEngine/Core/` offers more advanced and robust screenshot functionalities. The `debugContext` flag in `OpenGLProperties` is set to `true` conditionally based on the `SILICA_DEBUG` preprocessor macro within `Application.cpp`, ensuring it's only active in debug builds.