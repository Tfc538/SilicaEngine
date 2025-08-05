# Application Module

The `Application` module in SilicaEngine provides the core framework for managing the engine's lifecycle, windowing, and the main game loop. It serves as the entry point for any engine-based application, offering a flexible structure for custom game logic and event handling through virtual override methods.

## Key Concepts

*   **Application Lifecycle**: Manages initialization, the main update/render loop, and graceful shutdown.
*   **Configuration**: Allows customization of window properties, OpenGL versions, and other startup settings.
*   **Game Loop**: Provides a robust, high-precision loop for per-frame logic updates and rendering.
*   **Event Handling**: Integrates with the windowing system to process input (keyboard, mouse) and window events (resize, close).
*   **Extensibility**: Designed with virtual methods that can be overridden by derived application classes to inject custom game-specific logic.

## ApplicationConfig

This struct defines the initial configuration parameters for the `Application` instance. It allows developers to customize basic window properties and OpenGL context requirements.

```cpp
struct ApplicationConfig {
    std::string windowTitle = "SilicaEngine Application";
    uint32_t windowWidth = 1280;
    uint32_t windowHeight = 720;
    bool windowResizable = true;
    bool enableVSync = true;
    bool fullscreen = false;
    int openglMajorVersion = 4;
    int openglMinorVersion = 6;
};
```

## Public Interface

### Constructor and Destructor

```cpp
explicit Application(const ApplicationConfig& config = ApplicationConfig{});
virtual ~Application();
```
*   **`Application(const ApplicationConfig& config)`**: Constructs the application with specified settings. If no config is provided, default values are used.
*   **`~Application()`**: Ensures graceful shutdown if the application was initialized.

### Core Methods

```cpp
int Run();
void Close();
```
*   **`Run()`**: Starts the main application loop. This method handles initialization, the per-frame update/render cycle, and shutdown. Returns `0` on successful exit, `-1` on initialization failure.
*   **`Close()`**: Signals the application to initiate a graceful shutdown at the end of the current frame.

### Accessors

```cpp
Window& GetWindow() const;
bool IsRunning() const;
float GetTime() const;
float GetDeltaTime() const;
float GetFPS() const;
```
*   **`GetWindow()`**: Returns a reference to the `Window` object managed by the application.
*   **`IsRunning()`**: Checks if the application's main loop is currently active.
*   **`GetTime()`**: Returns the total application runtime in seconds since startup.
*   **`GetDeltaTime()`**: Returns the time elapsed (in seconds) since the last frame. Essential for frame-rate independent updates.
*   **`GetFPS()`**: Returns the current Frames Per Second (FPS) of the application, averaged over a short period.

## Virtual Methods (Override Points)

Derived application classes should override these methods to implement custom game logic and rendering. Base implementations provide default behaviors.

### Lifecycle Callbacks

```cpp
virtual ErrorResult<void> OnInitialize();
virtual void OnUpdate(float deltaTime);
virtual void OnRender();
virtual void OnDebugRender();
virtual void OnShutdown();
```
*   **`OnInitialize()`**: Called once after the window and OpenGL context are created. Use for application-specific setup. Returns `ErrorResult<void>` to indicate success or failure.
*   **`OnUpdate(float deltaTime)`**: Called once per frame for game logic updates. `deltaTime` provides frame-rate independent movement and simulation.
*   **`OnRender()`**: Called once per frame for rendering. Place your main drawing commands here.
*   **`OnDebugRender()`**: Called after `OnRender()` for any debug-specific drawing (e.g., collision shapes, profiler overlays).
*   **`OnShutdown()`**: Called once before the application fully shuts down. Use for application-specific cleanup.

### Event Callbacks

```cpp
virtual void OnWindowResize(uint32_t width, uint32_t height);
virtual void OnKeyEvent(int key, int scancode, int action, int mods);
virtual void OnMouseButtonEvent(int button, int action, int mods);
virtual void OnMouseMoveEvent(double xpos, double ypos);
virtual void OnScrollEvent(double xoffset, double yoffset);
```
These methods are called in response to user input and window events. Override them to implement custom input handling and dynamic window behaviors.

## Implementation Details

The `Application` class manages a `std::unique_ptr<Window>` for window creation and events. During `Initialize()`, it sets up GLFW callbacks to forward events to the appropriate `On...Event` virtual methods. Timing is handled by `UpdateTiming()`, which calculates delta time and FPS based on `glfwGetTime()`.

OpenGL debug context is conditionally enabled in debug builds using the `SILICA_DEBUG` preprocessor macro, ensuring no overhead in release builds. Similarly, `TRACE` and `INFO` level logging are compiled out in release builds.