/**
 * @file Input.h
 * @brief Comprehensive input management system for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Advanced input system with key remapping, context layers, and gamepad support.
 */

#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <array>
#include <mutex>

namespace SilicaEngine {

    /// Input action types
    enum class ActionType {
        Pressed,
        Released,
        Held
    };

    /// Input context types for layering input handling
    enum class InputContext : uint32_t {
        None = 0,
        Gameplay = 1,
        UI = 2,
        Debug = 4,
        Menu = 8,
        All = 15  // Combined value of all contexts (1+2+4+8)
    };

    /// Gamepad/controller button mappings
    enum class GamepadButton {
        A = GLFW_GAMEPAD_BUTTON_A,
        B = GLFW_GAMEPAD_BUTTON_B,
        X = GLFW_GAMEPAD_BUTTON_X,
        Y = GLFW_GAMEPAD_BUTTON_Y,
        LeftBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
        RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
        Back = GLFW_GAMEPAD_BUTTON_BACK,
        Start = GLFW_GAMEPAD_BUTTON_START,
        Guide = GLFW_GAMEPAD_BUTTON_GUIDE,
        LeftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
        RightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
        DpadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,
        DpadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
        DpadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
        DpadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT
    };

    /// Gamepad axis mappings
    enum class GamepadAxis {
        LeftX = GLFW_GAMEPAD_AXIS_LEFT_X,
        LeftY = GLFW_GAMEPAD_AXIS_LEFT_Y,
        RightX = GLFW_GAMEPAD_AXIS_RIGHT_X,
        RightY = GLFW_GAMEPAD_AXIS_RIGHT_Y,
        LeftTrigger = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
        RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER
    };

    /// Input source types
    enum class InputSource {
        Keyboard,
        Mouse,
        Gamepad
    };

    /// Input binding structure
    struct InputBinding {
        InputSource source;
        int code;                    // Key code, mouse button, or gamepad button
        int gamepadId = -1;          // For gamepad sources
        InputContext context = InputContext::Gameplay;
        std::string actionName;
        
        InputBinding() = default;
        InputBinding(InputSource src, int c, const std::string& action, InputContext ctx = InputContext::Gameplay)
            : source(src), code(c), actionName(action), context(ctx) {}
    };

    /// Gamepad state structure
    struct GamepadState {
        bool connected = false;
        std::array<bool, GLFW_GAMEPAD_BUTTON_LAST + 1> buttons{};
        std::array<float, GLFW_GAMEPAD_AXIS_LAST + 1> axes{};
        std::string name;
    };

    /// Action callback function type
    using ActionCallback = std::function<void(ActionType type, float value)>;

    /// Advanced Input Management System
    class Input {
    public:
        /// Initialize the input system
        static bool Initialize(GLFWwindow* window);
        
        /// Shutdown and cleanup
        static void Shutdown();
        
        /// Update input state (call once per frame)
        static void Update();
        
        // === Key/Button State Queries ===
        
        /// Check if a key is currently pressed
        static bool IsKeyPressed(int key);
        
        /// Check if a key was just pressed this frame
        static bool IsKeyJustPressed(int key);
        
        /// Check if a key was just released this frame
        static bool IsKeyJustReleased(int key);
        
        /// Check if a mouse button is currently pressed
        static bool IsMouseButtonPressed(int button);
        
        /// Check if a mouse button was just pressed this frame
        static bool IsMouseButtonJustPressed(int button);
        
        /// Check if a mouse button was just released this frame
        static bool IsMouseButtonJustReleased(int button);
        
        /// Get mouse position
        static void GetMousePosition(double& x, double& y);
        
        /// Get mouse delta (movement since last frame)
        static void GetMouseDelta(double& deltaX, double& deltaY);
        
        /// Get scroll wheel delta
        static void GetScrollDelta(double& deltaX, double& deltaY);
        
        // === Gamepad Support ===
        
        /// Check if gamepad is connected
        static bool IsGamepadConnected(int gamepadId);
        
        /// Get gamepad state
        static const GamepadState& GetGamepadState(int gamepadId);
        
        /// Check if gamepad button is pressed
        static bool IsGamepadButtonPressed(int gamepadId, GamepadButton button);
        
        /// Get gamepad axis value (-1.0 to 1.0)
        static float GetGamepadAxis(int gamepadId, GamepadAxis axis);
        
        /// Set gamepad deadzone for axes
        static void SetGamepadDeadzone(float deadzone);
        
        // === Action System ===
        
        /// Bind an input to an action
        static void BindAction(const std::string& actionName, const InputBinding& binding);
        
        /// Unbind action
        static void UnbindAction(const std::string& actionName);
        
        /// Check if action is active
        static bool IsActionActive(const std::string& actionName);
        
        /// Get action value (for analog inputs)
        static float GetActionValue(const std::string& actionName);
        
        /// Register action callback
        static void RegisterActionCallback(const std::string& actionName, ActionCallback callback);
        
        /// Unregister action callback
        static void UnregisterActionCallback(const std::string& actionName);
        
        // === Context Management ===
        
        /// Set active input context mask
        static void SetActiveContext(uint32_t contextMask);
        
        /// Add to active context
        static void AddActiveContext(InputContext context);
        
        /// Remove from active context
        static void RemoveActiveContext(InputContext context);
        
        /// Check if context is active
        static bool IsContextActive(InputContext context);
        
        // === Input Configuration ===
        
        /// Save input configuration to file
        static bool SaveConfiguration(const std::string& filename);
        
        /// Load input configuration from file
        static bool LoadConfiguration(const std::string& filename);
        
        /// Reset to default configuration
        static void ResetToDefaults();
        
        // === Debug/Utility ===
        
        /// Get key name from key code
        static const char* GetKeyName(int key);
        
        /// Get all connected gamepad names
        static std::vector<std::string> GetConnectedGamepadNames();
        
    private:
        // Static instance management
        static std::unique_ptr<class InputManager> s_Manager;
        
        // GLFW Callbacks
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void GamepadCallback(int gamepadId, int event);
    };

} // namespace SilicaEngine