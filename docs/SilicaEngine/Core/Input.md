# Input Module

The `Input` module in SilicaEngine provides a comprehensive, high-level input management system that abstracts keyboard, mouse, and gamepad input into a unified, configurable interface. It features an action-based binding system, input context layering, and robust gamepad support with automatic device detection and configuration persistence.

## Key Features

*   **Multi-Device Support**: Handles keyboard, mouse, and gamepad input through a unified API.
*   **Action-Based System**: Maps physical inputs to logical actions (e.g., "MoveForward", "Jump"), allowing for easy rebinding and cross-platform consistency.
*   **Input Context Layering**: Supports multiple input contexts (Gameplay, UI, Debug, Menu) that can be activated/deactivated to control which inputs are processed.
*   **Gamepad Support**: Full support for up to 16 gamepads with automatic connection/disconnection detection, deadzone configuration, and axis/button mapping.
*   **Configuration Persistence**: Save and load input configurations to/from files, enabling user-customizable controls.
*   **Callback System**: Register callbacks for action events (Pressed, Released, Held) to respond to input changes.
*   **Thread Safety**: All input operations are thread-safe using mutex protection.
*   **Frame-Accurate State Tracking**: Maintains both current and previous frame states for detecting "just pressed" and "just released" events.

## Core Concepts

### ActionType

Defines the different states an input action can be in:

```cpp
enum class ActionType {
    Pressed,    // Action was just activated this frame
    Released,   // Action was just deactivated this frame
    Held        // Action is currently active (may have been active for multiple frames)
};
```

### InputContext

Defines different input contexts that can be layered to control which inputs are active:

```cpp
enum class InputContext : uint32_t {
    None = 0,
    Gameplay = 1,
    UI = 2,
    Debug = 4,
    Menu = 8,
    All = 15  // Combined value of all contexts (1+2+4+8)
};
```

### InputSource

Specifies the type of input device:

```cpp
enum class InputSource {
    Keyboard,
    Mouse,
    Gamepad
};
```

## Data Structures

### InputBinding

Defines how a physical input maps to a logical action:

```cpp
struct InputBinding {
    InputSource source;           // Keyboard, Mouse, or Gamepad
    int code;                     // Key code, mouse button, or gamepad button/axis
    int gamepadId = -1;           // For gamepad sources (-1 for any gamepad)
    InputContext context = InputContext::Gameplay;
    std::string actionName;       // Logical action name (e.g., "MoveForward")
};
```

### GamepadState

Tracks the current state of a gamepad:

```cpp
struct GamepadState {
    bool connected = false;
    std::array<bool, GLFW_GAMEPAD_BUTTON_LAST + 1> buttons{};
    std::array<float, GLFW_GAMEPAD_AXIS_LAST + 1> axes{};
    std::string name;
};
```

## Public Interface

### Initialization and Lifecycle

```cpp
static bool Initialize(GLFWwindow* window);
static void Shutdown();
static void Update();
```

*   **`Initialize(GLFWwindow* window)`**: Sets up the input system, registers GLFW callbacks, and configures default bindings. Must be called before using any input functions.
*   **`Shutdown()`**: Cleans up the input system and unregisters GLFW callbacks.
*   **`Update()`**: Updates input states and processes action bindings. Should be called once per frame.

### Direct Input Queries

#### Keyboard Input

```cpp
static bool IsKeyPressed(int key);
static bool IsKeyJustPressed(int key);
static bool IsKeyJustReleased(int key);
```

*   **`IsKeyPressed(int key)`**: Returns `true` if the specified key is currently held down.
*   **`IsKeyJustPressed(int key)`**: Returns `true` if the key was pressed this frame (not held from previous frame).
*   **`IsKeyJustReleased(int key)`**: Returns `true` if the key was released this frame.

#### Mouse Input

```cpp
static bool IsMouseButtonPressed(int button);
static bool IsMouseButtonJustPressed(int button);
static bool IsMouseButtonJustReleased(int button);
static void GetMousePosition(double& x, double& y);
static void GetMouseDelta(double& deltaX, double& deltaY);
static void GetScrollDelta(double& deltaX, double& deltaY);
```

*   **`IsMouseButtonPressed/JustPressed/JustReleased(int button)`**: Similar to keyboard functions but for mouse buttons.
*   **`GetMousePosition(double& x, double& y)`**: Gets the current mouse cursor position in window coordinates.
*   **`GetMouseDelta(double& deltaX, double& deltaY)`**: Gets the mouse movement since the last frame.
*   **`GetScrollDelta(double& deltaX, double& deltaY)`**: Gets the scroll wheel movement since the last frame.

#### Gamepad Input

```cpp
static bool IsGamepadConnected(int gamepadId);
static const GamepadState& GetGamepadState(int gamepadId);
static bool IsGamepadButtonPressed(int gamepadId, GamepadButton button);
static float GetGamepadAxis(int gamepadId, GamepadAxis axis);
static void SetGamepadDeadzone(float deadzone);
```

*   **`IsGamepadConnected(int gamepadId)`**: Checks if a gamepad is connected.
*   **`GetGamepadState(int gamepadId)`**: Returns the complete state of a gamepad.
*   **`IsGamepadButtonPressed(int gamepadId, GamepadButton button)`**: Checks if a gamepad button is pressed.
*   **`GetGamepadAxis(int gamepadId, GamepadAxis axis)`**: Gets the value of a gamepad axis (-1.0 to 1.0).
*   **`SetGamepadDeadzone(float deadzone)`**: Sets the deadzone for analog inputs (0.0 to 1.0).

### Action System

```cpp
static void BindAction(const std::string& actionName, const InputBinding& binding);
static void UnbindAction(const std::string& actionName);
static bool IsActionActive(const std::string& actionName);
static float GetActionValue(const std::string& actionName);
static void RegisterActionCallback(const std::string& actionName, ActionCallback callback);
static void UnregisterActionCallback(const std::string& actionName);
```

*   **`BindAction(const std::string& actionName, const InputBinding& binding)`**: Maps a physical input to a logical action.
*   **`UnbindAction(const std::string& actionName)`**: Removes all bindings for an action.
*   **`IsActionActive(const std::string& actionName)`**: Returns `true` if any binding for the action is currently active.
*   **`GetActionValue(const std::string& actionName)`**: Returns the current value of an action (0.0 for inactive, 1.0 for digital active, or analog value for analog inputs).
*   **`RegisterActionCallback(const std::string& actionName, ActionCallback callback)`**: Registers a callback function to be called when the action state changes.
*   **`UnregisterActionCallback(const std::string& actionName)`**: Removes the callback for an action.

### Context Management

```cpp
static void SetActiveContext(uint32_t contextMask);
static void AddActiveContext(InputContext context);
static void RemoveActiveContext(InputContext context);
static bool IsContextActive(InputContext context);
```

*   **`SetActiveContext(uint32_t contextMask)`**: Sets the active input contexts using a bitmask.
*   **`AddActiveContext(InputContext context)`**: Adds a context to the active set.
*   **`RemoveActiveContext(InputContext context)`**: Removes a context from the active set.
*   **`IsContextActive(InputContext context)`**: Checks if a specific context is currently active.

### Configuration Management

```cpp
static bool SaveConfiguration(const std::string& filename);
static bool LoadConfiguration(const std::string& filename);
static void ResetToDefaults();
```

*   **`SaveConfiguration(const std::string& filename)`**: Saves the current input bindings to a file.
*   **`LoadConfiguration(const std::string& filename)`**: Loads input bindings from a file.
*   **`ResetToDefaults()`**: Restores the default input bindings.

## Usage Examples

### Basic Input Checking

```cpp
// Check if W key is pressed
if (SilicaEngine::Input::IsKeyPressed(GLFW_KEY_W)) {
    player.MoveForward();
}

// Check if mouse button was just pressed
if (SilicaEngine::Input::IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    player.Shoot();
}

// Get mouse movement for camera control
double mouseX, mouseY;
SilicaEngine::Input::GetMouseDelta(mouseX, mouseY);
camera.Rotate(mouseX, mouseY);
```

### Action-Based Input

```cpp
// Bind WASD keys to movement actions
SilicaEngine::Input::BindAction("MoveForward", 
    InputBinding(InputSource::Keyboard, GLFW_KEY_W, "MoveForward"));
SilicaEngine::Input::BindAction("MoveBackward", 
    InputBinding(InputSource::Keyboard, GLFW_KEY_S, "MoveBackward"));
SilicaEngine::Input::BindAction("MoveLeft", 
    InputBinding(InputSource::Keyboard, GLFW_KEY_A, "MoveLeft"));
SilicaEngine::Input::BindAction("MoveRight", 
    InputBinding(InputSource::Keyboard, GLFW_KEY_D, "MoveRight"));

// Check actions in game loop
if (SilicaEngine::Input::IsActionActive("MoveForward")) {
    player.MoveForward();
}
```

### Gamepad Support

```cpp
// Bind gamepad inputs
SilicaEngine::Input::BindAction("MoveForward", 
    InputBinding(InputSource::Gamepad, static_cast<int>(GamepadAxis::LeftY), "MoveForward"));
SilicaEngine::Input::BindAction("Jump", 
    InputBinding(InputSource::Gamepad, static_cast<int>(GamepadButton::A), "Jump"));

// Check gamepad connection
if (SilicaEngine::Input::IsGamepadConnected(0)) {
    float leftStickY = SilicaEngine::Input::GetGamepadAxis(0, GamepadAxis::LeftY);
    player.MoveForward(leftStickY);
}
```

### Input Contexts

```cpp
// Switch to UI context when menu is open
SilicaEngine::Input::SetActiveContext(static_cast<uint32_t>(InputContext::UI));

// Add debug context for debug controls
SilicaEngine::Input::AddActiveContext(InputContext::Debug);

// Check if debug context is active
if (SilicaEngine::Input::IsContextActive(InputContext::Debug)) {
    // Process debug-only inputs
}
```

### Action Callbacks

```cpp
// Register callback for jump action
SilicaEngine::Input::RegisterActionCallback("Jump", 
    [](ActionType type, float value) {
        if (type == ActionType::Pressed) {
            player.Jump();
        }
    });
```

## Implementation Details

The `Input` system uses a singleton pattern with an internal `InputManager` class that handles all the low-level details. It maintains arrays for current and previous frame states of keys and mouse buttons, allowing for accurate "just pressed" and "just released" detection.

Gamepad support is implemented using GLFW's gamepad API, with automatic detection of connected/disconnected devices. The system applies a configurable deadzone to analog inputs to prevent drift.

The action system processes all bindings each frame, checking if the binding's context is active and if the physical input is triggered. Actions can have multiple bindings, and the system will activate the action if any binding is active.

Thread safety is ensured through a mutex that protects all input state access, making the system safe for use in multi-threaded applications.

## Default Bindings

The system comes with sensible default bindings:

*   **Movement**: WASD keys for forward/backward/left/right movement
*   **Actions**: Space for jump, Left Shift for run
*   **Mouse**: Left button for fire, Right button for alt-fire
*   **Debug**: F1 for wireframe toggle, F3 for debug info toggle

These defaults can be overridden through the configuration system or by calling `BindAction` with new bindings. 