# Camera Module

The `Camera` module in SilicaEngine provides a comprehensive, configurable camera system that supports multiple projection types, movement modes, and input integration. It offers both programmatic control and automatic input processing, making it suitable for a wide range of applications from first-person games to 2D orthographic rendering.

## Key Features

*   **Multiple Projection Types**: Supports both perspective and orthographic projections with configurable parameters.
*   **Flexible Movement Modes**: Five distinct camera modes (FirstPerson, ThirdPerson, Orbital, Free, Fixed) for different use cases.
*   **Integrated Input Processing**: Automatic keyboard, mouse, and gamepad input handling with configurable sensitivity and controls.
*   **Smooth Movement**: Optional movement smoothing for more natural camera behavior.
*   **Constraint System**: Configurable pitch and yaw constraints to prevent unwanted camera orientations.
*   **Orbital Camera Support**: Full support for third-person and orbital cameras with distance control and target tracking.
*   **Screen-to-World Ray Casting**: Utility for converting screen coordinates to world space rays for picking and selection.
*   **Robust Error Handling**: Input validation and error recovery to prevent crashes from invalid parameters.

## Core Concepts

### CameraProjection

Defines the type of projection used by the camera:

```cpp
enum class CameraProjection {
    Perspective,    // 3D perspective projection with depth
    Orthographic    // 2D orthographic projection (no depth)
};
```

### CameraMode

Defines how the camera responds to input and movement:

```cpp
enum class CameraMode {
    FirstPerson,    // WASD movement + mouse look (like FPS games)
    ThirdPerson,    // Orbit around target with variable distance
    Orbital,        // Orbit around target with fixed distance
    Free,           // Full 6DOF movement (like 3D modeling software)
    Fixed           // No movement (static camera)
};
```

## CameraConfig

The `CameraConfig` struct provides comprehensive configuration options for all camera behaviors:

```cpp
struct CameraConfig {
    // Projection settings
    CameraProjection projection = CameraProjection::Perspective;
    float fov = 45.0f;                          // Field of view in degrees
    float nearPlane = 0.1f;                     // Near clipping plane
    float farPlane = 1000.0f;                   // Far clipping plane
    float orthoSize = 10.0f;                    // Orthographic size (half height)
    
    // Movement settings
    CameraMode mode = CameraMode::FirstPerson;
    float movementSpeed = 5.0f;                 // Units per second
    float rotationSpeed = 0.1f;                 // Radians per pixel
    float zoomSpeed = 2.0f;                     // Zoom sensitivity
    float mouseWheelZoomSpeed = 1.0f;           // Mouse wheel zoom sensitivity
    float orbitalRotationMultiplier = 10.0f;   // Orbital camera rotation speed
    
    // First person / Free mode settings
    float maxPitch = 89.0f;                     // Maximum pitch angle in degrees
    bool invertY = false;                       // Invert Y mouse axis
    bool smoothMovement = true;                 // Enable movement smoothing
    float smoothingFactor = 10.0f;              // Movement smoothing factor
    
    // Third person / Orbital mode settings
    glm::vec3 target = glm::vec3(0.0f);         // Target position to orbit around
    float distance = 10.0f;                     // Distance from target
    float minDistance = 1.0f;                   // Minimum orbit distance
    float maxDistance = 100.0f;                 // Maximum orbit distance
    
    // Constraints
    bool constrainPitch = true;                 // Constrain pitch rotation
    bool constrainYaw = false;                  // Constrain yaw rotation
    float minYaw = -180.0f;                     // Minimum yaw angle
    float maxYaw = 180.0f;                      // Maximum yaw angle
    
    // Input settings
    bool enableKeyboardMovement = true;
    bool enableMouseLook = true;
    bool enableMouseWheelZoom = true;
    bool enableGamepadMovement = true;
};
```

## Public Interface

### Constructor and Configuration

```cpp
explicit Camera(const CameraConfig& config = CameraConfig{});
void SetConfig(const CameraConfig& config);
const CameraConfig& GetConfig() const;
void SetEnabled(bool enabled);
bool IsEnabled() const;
```

*   **`Camera(const CameraConfig& config)`**: Creates a camera with the specified configuration.
*   **`SetConfig(const CameraConfig& config)`**: Updates the camera's configuration at runtime.
*   **`GetConfig()`**: Returns the current configuration.
*   **`SetEnabled(bool enabled)`**: Enables or disables camera processing.
*   **`IsEnabled()`**: Checks if the camera is currently enabled.

### Core Update Method

```cpp
void Update(float deltaTime);
```

*   **`Update(float deltaTime)`**: Main update method that processes input and updates camera state. Should be called once per frame.

### Position and Orientation

```cpp
void SetPosition(const glm::vec3& position);
const glm::vec3& GetPosition() const;
void SetRotation(float yaw, float pitch, float roll = 0.0f);
void SetYaw(float yaw);
void SetPitch(float pitch);
void SetRoll(float roll);
float GetYaw() const;
float GetPitch() const;
float GetRoll() const;
void SetDirection(const glm::vec3& direction);
glm::vec3 GetDirection() const;
void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
```

*   **`SetPosition(const glm::vec3& position)`**: Sets the camera's world position.
*   **`GetPosition()`**: Returns the camera's current position.
*   **`SetRotation(float yaw, float pitch, float roll)`**: Sets camera rotation using Euler angles (in degrees).
*   **`SetYaw/Pitch/Roll(float angle)`**: Sets individual rotation components.
*   **`GetYaw/Pitch/Roll()`**: Returns individual rotation components.
*   **`SetDirection(const glm::vec3& direction)`**: Sets camera orientation using a direction vector.
*   **`GetDirection()`**: Returns the camera's forward direction vector.
*   **`LookAt(const glm::vec3& target, const glm::vec3& up)`**: Orients the camera to look at a specific target.

### Projection Management

```cpp
void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
void SetOrthographic(float size, float aspectRatio, float nearPlane, float farPlane);
void SetAspectRatio(float aspectRatio);
float GetAspectRatio() const;
```

*   **`SetPerspective(...)`**: Configures perspective projection with field of view, aspect ratio, and clipping planes.
*   **`SetOrthographic(...)`**: Configures orthographic projection. Two overloads: one with explicit bounds, one with size and aspect ratio.
*   **`SetAspectRatio(float aspectRatio)`**: Updates the aspect ratio (useful when window is resized).
*   **`GetAspectRatio()`**: Returns the current aspect ratio.

### Matrix Access

```cpp
const glm::mat4& GetViewMatrix() const;
const glm::mat4& GetProjectionMatrix() const;
glm::mat4 GetViewProjectionMatrix() const;
```

*   **`GetViewMatrix()`**: Returns the current view matrix.
*   **`GetProjectionMatrix()`**: Returns the current projection matrix.
*   **`GetViewProjectionMatrix()`**: Returns the combined view-projection matrix.

### Programmatic Movement

```cpp
void Move(const glm::vec3& offset);
void MoveWorld(const glm::vec3& offset);
void Rotate(float deltaYaw, float deltaPitch, float deltaRoll = 0.0f);
void Zoom(float delta);
```

*   **`Move(const glm::vec3& offset)`**: Moves the camera in local space (relative to camera orientation).
*   **`MoveWorld(const glm::vec3& offset)`**: Moves the camera in world space (absolute movement).
*   **`Rotate(float deltaYaw, float deltaPitch, float deltaRoll)`**: Rotates the camera by the specified deltas.
*   **`Zoom(float delta)`**: Zooms the camera (changes FOV for perspective, ortho size for orthographic, or distance for orbital).

### Third Person / Orbital Mode

```cpp
void SetTarget(const glm::vec3& target);
const glm::vec3& GetTarget() const;
void SetDistance(float distance);
float GetDistance() const;
```

*   **`SetTarget(const glm::vec3& target)`**: Sets the target position for orbital cameras.
*   **`GetTarget()`**: Returns the current target position.
*   **`SetDistance(float distance)`**: Sets the distance from target for orbital cameras.
*   **`GetDistance()`**: Returns the current distance from target.

### Input Processing

```cpp
void ProcessKeyboardInput(float deltaTime);
void ProcessMouseMovement(float deltaX, float deltaY);
void ProcessMouseScroll(float deltaY);
void ProcessGamepadInput(float deltaTime);
```

These methods are automatically called by `Update()` when the corresponding input types are enabled in the configuration. They can also be called manually for custom input handling.

### Utility Methods

```cpp
glm::vec3 GetRight() const;
glm::vec3 GetUp() const;
glm::vec3 GetFront() const;
glm::vec3 ScreenToWorldRay(const glm::vec2& screenPos, const glm::vec2& screenSize) const;
```

*   **`GetRight()/GetUp()/GetFront()`**: Returns the camera's right, up, and forward vectors.
*   **`ScreenToWorldRay(const glm::vec2& screenPos, const glm::vec2& screenSize)`**: Converts screen coordinates to a world space ray for picking and selection.

## Usage Examples

### Basic Camera Setup

```cpp
// Create a first-person camera
CameraConfig config;
config.mode = CameraMode::FirstPerson;
config.movementSpeed = 10.0f;
config.rotationSpeed = 0.1f;
config.fov = 60.0f;

Camera camera(config);
camera.SetPosition(glm::vec3(0.0f, 2.0f, 5.0f));

// In game loop
camera.Update(deltaTime);
glm::mat4 viewProj = camera.GetViewProjectionMatrix();
```

### Third-Person Camera

```cpp
// Create a third-person camera
CameraConfig config;
config.mode = CameraMode::ThirdPerson;
config.target = glm::vec3(0.0f, 1.0f, 0.0f);  // Player position
config.distance = 8.0f;
config.minDistance = 2.0f;
config.maxDistance = 20.0f;

Camera camera(config);

// Update target as player moves
camera.SetTarget(player.GetPosition());
camera.Update(deltaTime);
```

### Orthographic Camera for 2D

```cpp
// Create an orthographic camera for 2D rendering
CameraConfig config;
config.projection = CameraProjection::Orthographic;
config.orthoSize = 10.0f;  // Half height of view

Camera camera(config);
camera.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));

// Update aspect ratio when window resizes
camera.SetAspectRatio(windowWidth / (float)windowHeight);
```

### Custom Input Handling

```cpp
// Disable automatic input processing
CameraConfig config;
config.enableKeyboardMovement = false;
config.enableMouseLook = false;

Camera camera(config);

// Handle input manually
if (Input::IsKeyPressed(GLFW_KEY_W)) {
    camera.Move(glm::vec3(0.0f, 0.0f, 1.0f) * deltaTime);
}

double mouseX, mouseY;
Input::GetMouseDelta(mouseX, mouseY);
camera.ProcessMouseMovement(mouseX, mouseY);
```

### Screen Picking

```cpp
// Convert mouse position to world ray
glm::vec2 mousePos(mouseX, mouseY);
glm::vec2 screenSize(windowWidth, windowHeight);
glm::vec3 rayDirection = camera.ScreenToWorldRay(mousePos, screenSize);

// Use ray for picking/selection
glm::vec3 rayOrigin = camera.GetPosition();
// ... perform ray casting with rayOrigin and rayDirection
```

## Implementation Details

The camera system uses Euler angles (yaw, pitch, roll) for rotation representation, which are converted to direction vectors for matrix calculations. The system automatically handles gimbal lock prevention by constraining pitch angles.

For orbital cameras, the system uses spherical coordinates to calculate the camera position relative to the target. The camera always looks at the target, maintaining proper orientation.

Input processing integrates with the `Input` module, using action-based input for movement and direct input queries for mouse and gamepad input. The system includes deadzone handling for gamepad input and configurable sensitivity for all input types.

The camera includes robust error handling, including validation of delta time values to prevent erratic behavior from frame rate spikes, and validation of screen coordinates for ray casting operations.

## Default Configuration

The camera comes with sensible defaults:

*   **Projection**: Perspective with 45° FOV
*   **Mode**: FirstPerson
*   **Movement**: WASD keys for movement, mouse for look
*   **Constraints**: Pitch constrained to ±89° to prevent gimbal lock
*   **Input**: All input types enabled by default

These defaults can be customized through the `CameraConfig` struct or by calling individual configuration methods. 