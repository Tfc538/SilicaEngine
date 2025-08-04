/**
 * @file Input.cpp
 * @brief Implementation of comprehensive input management system
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Core/Input.h"
#include "SilicaEngine/Core/Logger.h"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace SilicaEngine {

    /// Internal InputManager class implementation
    class InputManager {
    public:
        GLFWwindow* window = nullptr;
        
        // Key states (current and previous frame)
        std::array<bool, GLFW_KEY_LAST + 1> currentKeys{};
        std::array<bool, GLFW_KEY_LAST + 1> previousKeys{};
        
        // Mouse button states
        std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> currentMouseButtons{};
        std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> previousMouseButtons{};
        
        // Mouse state
        double mouseX = 0.0, mouseY = 0.0;
        double lastMouseX = 0.0, lastMouseY = 0.0;
        double mouseDeltaX = 0.0, mouseDeltaY = 0.0;
        double scrollDeltaX = 0.0, scrollDeltaY = 0.0;
        bool firstMouseInput = true;
        
        // Gamepad states
        std::array<GamepadState, GLFW_JOYSTICK_LAST + 1> gamepads{};
        float gamepadDeadzone = 0.15f;
        
        // Action system
        std::unordered_map<std::string, std::vector<InputBinding>> actionBindings;
        std::unordered_map<std::string, ActionCallback> actionCallbacks;
        std::unordered_map<std::string, bool> actionStates;
        std::unordered_map<std::string, float> actionValues;
        
        // Context management
        uint32_t activeContextMask = static_cast<uint32_t>(InputContext::Gameplay);
        
        // Thread safety
        mutable std::mutex inputMutex;
        
        void UpdateGamepads() {
            for (int gamepadId = 0; gamepadId <= GLFW_JOYSTICK_LAST; gamepadId++) {
                bool wasConnected = gamepads[gamepadId].connected;
                bool isConnected = glfwJoystickPresent(gamepadId) && glfwJoystickIsGamepad(gamepadId);
                
                if (isConnected != wasConnected) {
                    if (isConnected) {
                        SE_INFO("Gamepad {} connected: {}", gamepadId, glfwGetGamepadName(gamepadId));
                        gamepads[gamepadId].name = glfwGetGamepadName(gamepadId);
                    } else {
                        SE_INFO("Gamepad {} disconnected", gamepadId);
                        gamepads[gamepadId].name.clear();
                    }
                    gamepads[gamepadId].connected = isConnected;
                }
                
                if (isConnected) {
                    GLFWgamepadstate state;
                    if (glfwGetGamepadState(gamepadId, &state)) {
                        // Update button states
                        for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) {
                            gamepads[gamepadId].buttons[i] = state.buttons[i] == GLFW_PRESS;
                        }
                        
                        // Update axis states with deadzone
                        for (int i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; i++) {
                            float value = state.axes[i];
                            if (std::abs(value) < gamepadDeadzone) {
                                value = 0.0f;
                            }
                            gamepads[gamepadId].axes[i] = value;
                        }
                    }
                }
            }
        }
        
        void UpdateActions() {
            // Clear previous frame scroll deltas
            scrollDeltaX = 0.0;
            scrollDeltaY = 0.0;
            
            // Process all action bindings
            for (auto& [actionName, bindings] : actionBindings) {
                bool actionActive = false;
                float actionValue = 0.0f;
                bool prevState = actionStates[actionName];
                
                for (const auto& binding : bindings) {
                    // Check if this binding's context is active
                    if (!(activeContextMask & static_cast<uint32_t>(binding.context))) {
                        continue;
                    }
                    
                    bool bindingActive = false;
                    float bindingValue = 0.0f;
                    
                    switch (binding.source) {
                        case InputSource::Keyboard:
                            bindingActive = currentKeys[binding.code];
                            bindingValue = bindingActive ? 1.0f : 0.0f;
                            break;
                            
                        case InputSource::Mouse:
                            bindingActive = currentMouseButtons[binding.code];
                            bindingValue = bindingActive ? 1.0f : 0.0f;
                            break;
                            
                        case InputSource::Gamepad:
                            if (binding.gamepadId >= 0 && binding.gamepadId <= GLFW_JOYSTICK_LAST &&
                                gamepads[binding.gamepadId].connected) {
                                if (binding.code <= GLFW_GAMEPAD_BUTTON_LAST) {
                                    // Button input
                                    bindingActive = gamepads[binding.gamepadId].buttons[binding.code];
                                    bindingValue = bindingActive ? 1.0f : 0.0f;
                                } else {
                                    if (binding.code > GLFW_GAMEPAD_BUTTON_LAST) {
                                        int axisCode = binding.code - (GLFW_GAMEPAD_BUTTON_LAST + 1);
                                        if (axisCode >= 0 && axisCode <= GLFW_GAMEPAD_AXIS_LAST) {
                                            bindingValue = gamepads[binding.gamepadId].axes[axisCode];
                                            bindingActive = std::abs(bindingValue) > 0.0f;
                                        }
                                    }
                                }
                            }
                            break;
                    }
                    
                    if (bindingActive) {
                        actionActive = true;
                        actionValue = std::max(actionValue, std::abs(bindingValue));
                    }
                }
                
                // Update action state
                actionStates[actionName] = actionActive;
                actionValues[actionName] = actionValue;
                
                // Trigger callbacks for state changes
                auto callbackIt = actionCallbacks.find(actionName);
                if (callbackIt != actionCallbacks.end()) {
                    if (actionActive && !prevState) {
                        callbackIt->second(ActionType::Pressed, actionValue);
                    } else if (!actionActive && prevState) {
                        callbackIt->second(ActionType::Released, 0.0f);
                    } else if (actionActive) {
                        callbackIt->second(ActionType::Held, actionValue);
                    }
                }
            }
        }
        
        void SetupDefaultBindings() {
            // Default gameplay bindings
            BindAction("MoveForward", InputBinding(InputSource::Keyboard, GLFW_KEY_W, "MoveForward"));
            BindAction("MoveBackward", InputBinding(InputSource::Keyboard, GLFW_KEY_S, "MoveBackward"));
            BindAction("MoveLeft", InputBinding(InputSource::Keyboard, GLFW_KEY_A, "MoveLeft"));
            BindAction("MoveRight", InputBinding(InputSource::Keyboard, GLFW_KEY_D, "MoveRight"));
            BindAction("Jump", InputBinding(InputSource::Keyboard, GLFW_KEY_SPACE, "Jump"));
            BindAction("Run", InputBinding(InputSource::Keyboard, GLFW_KEY_LEFT_SHIFT, "Run"));
            
            // Mouse bindings
            BindAction("Fire", InputBinding(InputSource::Mouse, GLFW_MOUSE_BUTTON_LEFT, "Fire"));
            BindAction("AltFire", InputBinding(InputSource::Mouse, GLFW_MOUSE_BUTTON_RIGHT, "AltFire"));
            
            // Debug context bindings
            BindAction("ToggleWireframe", InputBinding(InputSource::Keyboard, GLFW_KEY_F1, "ToggleWireframe", InputContext::Debug));
            BindAction("ToggleDebugInfo", InputBinding(InputSource::Keyboard, GLFW_KEY_F3, "ToggleDebugInfo", InputContext::Debug));
            
            SE_INFO("Default input bindings configured");
        }
        
        void BindAction(const std::string& actionName, const InputBinding& binding) {
            actionBindings[actionName].push_back(binding);
            actionStates[actionName] = false;
            actionValues[actionName] = 0.0f;
        }
    };

    // Static instance
    std::unique_ptr<InputManager> Input::s_Manager = nullptr;

    bool Input::Initialize(GLFWwindow* window) {
        if (s_Manager) {
            SE_WARN("Input system already initialized");
            return true;
        }
        
        s_Manager = std::make_unique<InputManager>();
        s_Manager->window = window;
        
        // Set GLFW callbacks
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetCursorPosCallback(window, CursorPosCallback);
        glfwSetScrollCallback(window, ScrollCallback);
        glfwSetJoystickCallback(GamepadCallback);
        
        // Initialize gamepad states
        for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++) {
            s_Manager->gamepads[i].connected = glfwJoystickPresent(i) && glfwJoystickIsGamepad(i);
            if (s_Manager->gamepads[i].connected) {
                s_Manager->gamepads[i].name = glfwGetGamepadName(i);
                SE_INFO("Gamepad {} detected: {}", i, s_Manager->gamepads[i].name);
            }
        }
        
        // Setup default bindings
        s_Manager->SetupDefaultBindings();
        
        SE_INFO("Input system initialized successfully");
        return true;
    }
    
    void Input::Shutdown() {
        if (s_Manager) {
            SE_INFO("Input system shutting down");
            s_Manager.reset();
        }
    }
    
    void Input::Update() {
        if (!s_Manager) return;
        
        // Copy current state to previous
        s_Manager->previousKeys = s_Manager->currentKeys;
        s_Manager->previousMouseButtons = s_Manager->currentMouseButtons;
        
        // Update mouse delta
        s_Manager->mouseDeltaX = s_Manager->mouseX - s_Manager->lastMouseX;
        s_Manager->mouseDeltaY = s_Manager->mouseY - s_Manager->lastMouseY;
        s_Manager->lastMouseX = s_Manager->mouseX;
        s_Manager->lastMouseY = s_Manager->mouseY;
        
        // Handle first mouse input to avoid large delta
        if (s_Manager->firstMouseInput) {
            s_Manager->mouseDeltaX = 0.0;
            s_Manager->mouseDeltaY = 0.0;
            s_Manager->firstMouseInput = false;
        }
        
        // Update gamepad states
        s_Manager->UpdateGamepads();
        
        // Update action system
        s_Manager->UpdateActions();
    }
    
    // === Key/Button State Queries ===
    
    bool Input::IsKeyPressed(int key) {
        if (!s_Manager || key < 0 || key > GLFW_KEY_LAST) return false;
        std::lock_guard<std::mutex> lock(s_Manager->inputMutex);
        return s_Manager->currentKeys[key];
    }
    
    bool Input::IsKeyJustPressed(int key) {
        if (!s_Manager || key < 0 || key > GLFW_KEY_LAST) return false;
        std::lock_guard<std::mutex> lock(s_Manager->inputMutex);
        return s_Manager->currentKeys[key] && !s_Manager->previousKeys[key];
    }
    
    bool Input::IsKeyJustReleased(int key) {
        if (!s_Manager || key < 0 || key > GLFW_KEY_LAST) return false;
        std::lock_guard<std::mutex> lock(s_Manager->inputMutex);
        return !s_Manager->currentKeys[key] && s_Manager->previousKeys[key];
    }
    
    bool Input::IsMouseButtonPressed(int button) {
        if (!s_Manager || button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
        return s_Manager->currentMouseButtons[button];
    }
    
    bool Input::IsMouseButtonJustPressed(int button) {
        if (!s_Manager || button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
        return s_Manager->currentMouseButtons[button] && !s_Manager->previousMouseButtons[button];
    }
    
    bool Input::IsMouseButtonJustReleased(int button) {
        if (!s_Manager || button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
        return !s_Manager->currentMouseButtons[button] && s_Manager->previousMouseButtons[button];
    }
    
    void Input::GetMousePosition(double& x, double& y) {
        if (!s_Manager) {
            x = y = 0.0;
            return;
        }
        x = s_Manager->mouseX;
        y = s_Manager->mouseY;
    }
    
    void Input::GetMouseDelta(double& deltaX, double& deltaY) {
        if (!s_Manager) {
            deltaX = deltaY = 0.0;
            return;
        }
        deltaX = s_Manager->mouseDeltaX;
        deltaY = s_Manager->mouseDeltaY;
    }
    
    void Input::GetScrollDelta(double& deltaX, double& deltaY) {
        if (!s_Manager) {
            deltaX = deltaY = 0.0;
            return;
        }
        deltaX = s_Manager->scrollDeltaX;
        deltaY = s_Manager->scrollDeltaY;
    }
    
    // === Gamepad Support ===
    
    bool Input::IsGamepadConnected(int gamepadId) {
        if (!s_Manager || gamepadId < 0 || gamepadId > GLFW_JOYSTICK_LAST) return false;
        return s_Manager->gamepads[gamepadId].connected;
    }
    
    const GamepadState& Input::GetGamepadState(int gamepadId) {
        static GamepadState emptyState{};
        if (!s_Manager || gamepadId < 0 || gamepadId > GLFW_JOYSTICK_LAST) return emptyState;
        return s_Manager->gamepads[gamepadId];
    }
    
    bool Input::IsGamepadButtonPressed(int gamepadId, GamepadButton button) {
        if (!s_Manager || gamepadId < 0 || gamepadId > GLFW_JOYSTICK_LAST) return false;
        if (!s_Manager->gamepads[gamepadId].connected) return false;
        return s_Manager->gamepads[gamepadId].buttons[static_cast<int>(button)];
    }
    
    float Input::GetGamepadAxis(int gamepadId, GamepadAxis axis) {
        if (!s_Manager || gamepadId < 0 || gamepadId > GLFW_JOYSTICK_LAST) return 0.0f;
        if (!s_Manager->gamepads[gamepadId].connected) return 0.0f;
        return s_Manager->gamepads[gamepadId].axes[static_cast<int>(axis)];
    }
    
    void Input::SetGamepadDeadzone(float deadzone) {
        if (!s_Manager) return;
        s_Manager->gamepadDeadzone = std::clamp(deadzone, 0.0f, 1.0f);
    }
    
    // === Action System ===
    
    void Input::BindAction(const std::string& actionName, const InputBinding& binding) {
        if (!s_Manager) return;
        s_Manager->BindAction(actionName, binding);
    }
    
    void Input::UnbindAction(const std::string& actionName) {
        if (!s_Manager) return;
        s_Manager->actionBindings.erase(actionName);
        s_Manager->actionStates.erase(actionName);
        s_Manager->actionValues.erase(actionName);
        s_Manager->actionCallbacks.erase(actionName);
    }
    
    bool Input::IsActionActive(const std::string& actionName) {
        if (!s_Manager) return false;
        auto it = s_Manager->actionStates.find(actionName);
        return it != s_Manager->actionStates.end() ? it->second : false;
    }
    
    float Input::GetActionValue(const std::string& actionName) {
        if (!s_Manager) return 0.0f;
        auto it = s_Manager->actionValues.find(actionName);
        return it != s_Manager->actionValues.end() ? it->second : 0.0f;
    }
    
    void Input::RegisterActionCallback(const std::string& actionName, ActionCallback callback) {
        if (!s_Manager) return;
        s_Manager->actionCallbacks[actionName] = callback;
    }
    
    void Input::UnregisterActionCallback(const std::string& actionName) {
        if (!s_Manager) return;
        s_Manager->actionCallbacks.erase(actionName);
    }
    
    // === Context Management ===
    
    void Input::SetActiveContext(uint32_t contextMask) {
        if (!s_Manager) return;
        s_Manager->activeContextMask = contextMask;
    }
    
    void Input::AddActiveContext(InputContext context) {
        if (!s_Manager) return;
        s_Manager->activeContextMask |= static_cast<uint32_t>(context);
    }
    
    void Input::RemoveActiveContext(InputContext context) {
        if (!s_Manager) return;
        s_Manager->activeContextMask &= ~static_cast<uint32_t>(context);
    }
    
    bool Input::IsContextActive(InputContext context) {
        if (!s_Manager) return false;
        return (s_Manager->activeContextMask & static_cast<uint32_t>(context)) != 0;
    }
    
    // === Input Configuration ===
    
    bool Input::SaveConfiguration(const std::string& filename) {
        if (!s_Manager) return false;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            SE_ERROR("Failed to open input configuration file for writing: {}", filename);
            return false;
        }
        
        file << "# SilicaEngine Input Configuration\n";
        file << "# Format: ActionName,SourceType,Code,GamepadId,Context\n\n";
        
        for (const auto& [actionName, bindings] : s_Manager->actionBindings) {
            for (const auto& binding : bindings) {
                file << actionName << ","
                     << static_cast<int>(binding.source) << ","
                     << binding.code << ","
                     << binding.gamepadId << ","
                     << static_cast<int>(binding.context) << "\n";
            }
        }
        
        SE_INFO("Input configuration saved to: {}", filename);
        return true;
    }
    
    bool Input::LoadConfiguration(const std::string& filename) {
        if (!s_Manager) return false;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            SE_WARN("Failed to open input configuration file: {}", filename);
            return false;
        }
        
        // Clear existing bindings
        s_Manager->actionBindings.clear();
        s_Manager->actionStates.clear();
        s_Manager->actionValues.clear();
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 5) {
                InputBinding binding;
                binding.actionName = tokens[0];
                
                // Validate and parse integer values safely
                try {
                    int sourceValue = std::stoi(tokens[1]);
                    int codeValue = std::stoi(tokens[2]);
                    int gamepadIdValue = std::stoi(tokens[3]);
                    int contextValue = std::stoi(tokens[4]);
                    
                    // Validate ranges for enum values
                    if (sourceValue < 0 || sourceValue > static_cast<int>(InputSource::Gamepad) ||
                        codeValue < 0 || 
                        gamepadIdValue < 0 || gamepadIdValue >= 16 || // GLFW supports max 16 gamepads
                        contextValue < 0 || contextValue > static_cast<int>(InputContext::All)) {
                        SE_WARN("Invalid input binding values in config: source={}, code={}, gamepadId={}, context={}", 
                                sourceValue, codeValue, gamepadIdValue, contextValue);
                        continue; // Skip this invalid binding
                    }
                    
                    binding.source = static_cast<InputSource>(sourceValue);
                    binding.code = codeValue;
                    binding.gamepadId = gamepadIdValue;
                    binding.context = static_cast<InputContext>(contextValue);
                } catch (const std::exception& e) {
                    SE_WARN("Failed to parse input binding integers: {}. Skipping binding.", e.what());
                    continue; // Skip this invalid binding
                }
                
                s_Manager->BindAction(binding.actionName, binding);
            }
        }
        
        SE_INFO("Input configuration loaded from: {}", filename);
        return true;
    }
    
    void Input::ResetToDefaults() {
        if (!s_Manager) return;
        
        s_Manager->actionBindings.clear();
        s_Manager->actionStates.clear();
        s_Manager->actionValues.clear();
        s_Manager->actionCallbacks.clear();
        
        s_Manager->SetupDefaultBindings();
        SE_INFO("Input configuration reset to defaults");
    }
    
    // === Debug/Utility ===
    
    const char* Input::GetKeyName(int key) {
        return glfwGetKeyName(key, 0);
    }
    
    std::vector<std::string> Input::GetConnectedGamepadNames() {
        std::vector<std::string> names;
        if (!s_Manager) return names;
        
        for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++) {
            if (s_Manager->gamepads[i].connected) {
                names.push_back(s_Manager->gamepads[i].name);
            }
        }
        return names;
    }
    
    // === GLFW Callbacks ===
    
    void Input::KeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/) {
        if (!s_Manager || key < 0 || key > GLFW_KEY_LAST) return;
        
        std::lock_guard<std::mutex> lock(s_Manager->inputMutex);
        if (action == GLFW_PRESS) {
            s_Manager->currentKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            s_Manager->currentKeys[key] = false;
        }
    }
    
    void Input::MouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
        if (!s_Manager || button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return;
        
        std::lock_guard<std::mutex> lock(s_Manager->inputMutex);
        if (action == GLFW_PRESS) {
            s_Manager->currentMouseButtons[button] = true;
        } else if (action == GLFW_RELEASE) {
            s_Manager->currentMouseButtons[button] = false;
        }
    }
    
    void Input::CursorPosCallback(GLFWwindow* /*window*/, double xpos, double ypos) {
        if (!s_Manager) return;
        std::lock_guard<std::mutex> lock(s_Manager->inputMutex);
        s_Manager->mouseX = xpos;
        s_Manager->mouseY = ypos;
    }
    
    void Input::ScrollCallback(GLFWwindow* /*window*/, double xoffset, double yoffset) {
        if (!s_Manager) return;
        std::lock_guard<std::mutex> lock(s_Manager->inputMutex);
        s_Manager->scrollDeltaX = xoffset;
        s_Manager->scrollDeltaY = yoffset;
    }
    
    void Input::GamepadCallback(int gamepadId, int event) {
        if (!s_Manager) return;
        
        if (event == GLFW_CONNECTED) {
            if (glfwJoystickIsGamepad(gamepadId)) {
                s_Manager->gamepads[gamepadId].connected = true;
                s_Manager->gamepads[gamepadId].name = glfwGetGamepadName(gamepadId);
                SE_INFO("Gamepad {} connected: {}", gamepadId, s_Manager->gamepads[gamepadId].name);
            }
        } else if (event == GLFW_DISCONNECTED) {
            s_Manager->gamepads[gamepadId].connected = false;
            SE_INFO("Gamepad {} disconnected", gamepadId);
        }
    }

} // namespace SilicaEngine