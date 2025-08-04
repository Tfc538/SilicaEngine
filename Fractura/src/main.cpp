/**
 * @file main.cpp
 * @brief Fractura: Shatter Garden
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 2.0.0
 * 
 * A visually rich, procedurally animated cube garden.
 * Walk through a surreal world of shifting, glowing cubes.
 */

#include <SilicaEngine/SilicaEngine.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <unordered_map> // Added for noise caching

/// Animation modes for the cube garden
enum class AnimationMode {
    Wave,      // Sine wave ripples
    Pulse,     // Radial pulsing from center
    Orbit,     // Cubes orbit around invisible points
    Chaos      // Random chaotic movement
};

/// Individual cube in the garden
struct GardenCube {
    glm::vec3 basePosition;    // Original grid position
    glm::vec3 currentPosition; // Current animated position
    glm::vec4 baseColor;       // Base color
    glm::vec4 currentColor;    // Current animated color
    float animationOffset;     // Unique animation offset
    float scale;               // Current scale
    float rotation;            // Current rotation
    
    GardenCube(const glm::vec3& pos, const glm::vec4& col, float offset)
        : basePosition(pos), currentPosition(pos), baseColor(col), currentColor(col)
        , animationOffset(offset), scale(1.0f), rotation(0.0f) {}
};

/// Fractura: Shatter Garden
class FracturaShatterGarden : public SilicaEngine::Application {
public:
    FracturaShatterGarden() 
        : SilicaEngine::Application(GetGardenConfig())
        , m_CameraPosition(75.0f, 50.0f, 75.0f)  // Start higher for massive garden
        , m_CameraRotation(-45.0f, -20.0f)
        , m_CameraSpeed(25.0f)  // Faster movement for larger space
        , m_MouseSensitivity(0.1f)
        , m_Time(0.0f)
        , m_TimeFrozen(false)
        , m_CurrentMode(AnimationMode::Wave)
        , m_LastMouseX(0.0)
        , m_LastMouseY(0.0)
        , m_FirstMouse(true)
        , m_WireframeMode(false)
        , m_ShowStats(true)
        , m_FlyMode(true) {
        
        GenerateGarden();
    }

protected:
    SilicaEngine::ErrorResult<void> OnInitialize() override {
        SE_APP_INFO("Initializing Fractura: Shatter Garden...");
        
        // Setup camera
        UpdateCameraMatrices();
        
        // Capture mouse for camera control
        GetWindow().SetCursorMode(GLFW_CURSOR_DISABLED);
        
        // Get initial mouse position
        GetWindow().GetCursorPos(m_LastMouseX, m_LastMouseY);
        
        SE_APP_INFO("Fractura: Shatter Garden initialized successfully!");
        SE_APP_INFO("Welcome to the Shatter Garden - a world of animated cubes!");
        SE_APP_INFO("Controls:");
        SE_APP_INFO("  WASD - Move camera");
        SE_APP_INFO("  Mouse - Look around");
        SE_APP_INFO("  F - Toggle wireframe mode");
        SE_APP_INFO("  G - Regenerate garden");
        SE_APP_INFO("  T - Toggle time freeze");
        SE_APP_INFO("  1/2/3/4 - Switch animation modes");
        SE_APP_INFO("  F2 - Toggle statistics");
        SE_APP_INFO("  ESC - Exit");
        SE_APP_INFO("Enjoy the procedural cube garden!");
        
        return SilicaEngine::ErrorResult<void>::Success();
    }
    
    void OnUpdate(float deltaTime) override {
        // Update time (or keep frozen)
        if (!m_TimeFrozen) {
            m_Time += deltaTime;
        }
        
        // Handle input
        HandleInput(deltaTime);
        
        // Update camera matrices
        UpdateCameraMatrices();
        
        // Update cube animations
        UpdateGardenAnimation();
    }
    
    void OnRender() override {
        // Clear screen with atmospheric color
        glm::vec3 clearColor = GetAtmosphericColor();
        SilicaEngine::Renderer::SetClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        SilicaEngine::Renderer::Clear();
        
        // Draw the garden using batch rendering
        SilicaEngine::Renderer::BeginCubeBatch();
        
        // Render all garden cubes
        for (const auto& cube : m_GardenCubes) {
            glm::vec3 size(cube.scale);
            SilicaEngine::Renderer::AddCubeToBatch(cube.currentPosition, size, cube.currentColor);
        }
        
        // Flush batch
        SilicaEngine::Renderer::EndCubeBatch();
        
        // Draw statistics if enabled
        if (m_ShowStats) {
            DrawStatistics();
        }
    }
    
    void OnKeyEvent(int key, int scancode, int action, int mods) override {
        // Call base class first
        SilicaEngine::Application::OnKeyEvent(key, scancode, action, mods);
        
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_F:
                    m_WireframeMode = !m_WireframeMode;
                    SilicaEngine::Renderer::SetWireframe(m_WireframeMode);
                    SE_APP_INFO("Wireframe mode: {}", m_WireframeMode ? "ON" : "OFF");
                    break;
                    
                case GLFW_KEY_F2:
                    m_ShowStats = !m_ShowStats;
                    SE_APP_INFO("Statistics display: {}", m_ShowStats ? "ON" : "OFF");
                    break;
                    
                case GLFW_KEY_G:
                    GenerateGarden();
                    SE_APP_INFO("Garden regenerated!");
                    break;
                    
                case GLFW_KEY_T:
                    m_TimeFrozen = !m_TimeFrozen;
                    SE_APP_INFO("Time: {}", m_TimeFrozen ? "FROZEN" : "FLOWING");
                    break;
                    
                case GLFW_KEY_1:
                    m_CurrentMode = AnimationMode::Wave;
                    SE_APP_INFO("Animation mode: WAVE");
                    break;
                    
                case GLFW_KEY_2:
                    m_CurrentMode = AnimationMode::Pulse;
                    SE_APP_INFO("Animation mode: PULSE");
                    break;
                    
                case GLFW_KEY_3:
                    m_CurrentMode = AnimationMode::Orbit;
                    SE_APP_INFO("Animation mode: ORBIT");
                    break;
                    
                case GLFW_KEY_4:
                    m_CurrentMode = AnimationMode::Chaos;
                    SE_APP_INFO("Animation mode: CHAOS");
                    break;
            }
        }
    }
    
    void OnMouseMoveEvent(double xpos, double ypos) override {
        if (m_FirstMouse) {
            m_LastMouseX = xpos;
            m_LastMouseY = ypos;
            m_FirstMouse = false;
        }
        
        double xoffset = xpos - m_LastMouseX;
        double yoffset = m_LastMouseY - ypos; // Reversed since y-coordinates go from bottom to top
        
        m_LastMouseX = xpos;
        m_LastMouseY = ypos;
        
        xoffset *= m_MouseSensitivity;
        yoffset *= m_MouseSensitivity;
        
        m_CameraRotation.x += static_cast<float>(xoffset);
        m_CameraRotation.y += static_cast<float>(yoffset);
        
        // Constrain pitch
        if (m_CameraRotation.y > 89.0f) m_CameraRotation.y = 89.0f;
        if (m_CameraRotation.y < -89.0f) m_CameraRotation.y = -89.0f;
    }

private:
    // Camera properties
    glm::vec3 m_CameraPosition;
    glm::vec2 m_CameraRotation; // x = yaw, y = pitch
    float m_CameraSpeed;
    float m_MouseSensitivity;
    
    // Time and animation state
    float m_Time;
    bool m_TimeFrozen;
    AnimationMode m_CurrentMode;
    
    // Mouse input state
    double m_LastMouseX, m_LastMouseY;
    bool m_FirstMouse;
    
    // Garden data
    std::vector<GardenCube> m_GardenCubes;
    
    // Noise caching for Chaos mode performance
    std::unordered_map<std::string, float> m_NoiseCache;
    float m_LastCacheTime = 0.0f;
    static constexpr float CACHE_DURATION = 0.1f; // Cache for 100ms
    
    // Rendering state
    bool m_WireframeMode;
    bool m_ShowStats;
    bool m_FlyMode;
    
    // Garden configuration
    static constexpr int GARDEN_SIZE_X = 30;
    static constexpr int GARDEN_SIZE_Z = 30;
    static constexpr int GARDEN_SIZE_Y = 3;
    static constexpr float CUBE_SPACING = 2.0f;
    
    /**
     * @brief Get application configuration for the garden
     */
    static SilicaEngine::ApplicationConfig GetGardenConfig() {
        SilicaEngine::ApplicationConfig config;
        config.windowTitle = "Fractura: Shatter Garden";
        config.windowWidth = 1920;
        config.windowHeight = 1080;
        config.windowResizable = true;
        config.enableVSync = true;
        config.openglMajorVersion = 4;
        config.openglMinorVersion = 6;
        return config;
    }
    
    /**
     * @brief Generate the procedural cube garden
     */
    void GenerateGarden() {
        m_GardenCubes.clear();
        m_GardenCubes.reserve(GARDEN_SIZE_X * GARDEN_SIZE_Z * GARDEN_SIZE_Y);
        
        // Robust random seeding with fallback mechanism
        std::random_device rd;
        std::mt19937 gen;
        
        try {
            // Try to get a valid seed from random_device
            unsigned int seed = rd();
            if (seed == 0) {
                // If random_device returns 0, it might be broken on this platform
                throw std::runtime_error("Random device returned 0");
            }
            gen.seed(seed);
            SE_APP_INFO("Using random_device seed: {}", seed);
        } catch (const std::exception& e) {
            // Fallback to time-based seed with additional entropy
            auto now = std::chrono::high_resolution_clock::now();
            auto timeSeed = static_cast<unsigned int>(now.time_since_epoch().count());
            
            // Add additional entropy from system info
            auto pid = static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            unsigned int fallbackSeed = timeSeed ^ pid ^ 0xDEADBEEF;
            
            gen.seed(fallbackSeed);
            SE_APP_WARN("Random device failed, using fallback seed: {} (error: {})", fallbackSeed, e.what());
        }
        
        std::uniform_real_distribution<float> colorDist(0.3f, 1.0f);
        std::uniform_real_distribution<float> offsetDist(0.0f, 6.28f); // 0 to 2*PI
        
        for (int x = 0; x < GARDEN_SIZE_X; ++x) {
            for (int z = 0; z < GARDEN_SIZE_Z; ++z) {
                for (int y = 0; y < GARDEN_SIZE_Y; ++y) {
                    // Calculate world position
                    glm::vec3 worldPos(
                        (x - GARDEN_SIZE_X / 2) * CUBE_SPACING,
                        y * CUBE_SPACING,
                        (z - GARDEN_SIZE_Z / 2) * CUBE_SPACING
                    );
                    
                    // Generate vibrant color based on position
                    float hue = (x + z + y * 0.5f) / (GARDEN_SIZE_X + GARDEN_SIZE_Z);
                    glm::vec4 color = HSVtoRGB(hue * 360.0f, 0.8f, colorDist(gen));
                    
                    // Unique animation offset for each cube
                    float animOffset = offsetDist(gen);
                    
                    m_GardenCubes.emplace_back(worldPos, color, animOffset);
                }
            }
        }
        
        SE_APP_INFO("Generated garden with {} cubes", m_GardenCubes.size());
    }
    
    /**
     * @brief Handle keyboard input for camera movement
     */
    void HandleInput(float deltaTime) {
        GLFWwindow* window = GetWindow().GetNativeWindow();
        
        // Calculate camera direction vectors
        glm::vec3 front;
        front.x = cos(glm::radians(m_CameraRotation.x)) * cos(glm::radians(m_CameraRotation.y));
        front.y = sin(glm::radians(m_CameraRotation.y));
        front.z = sin(glm::radians(m_CameraRotation.x)) * cos(glm::radians(m_CameraRotation.y));
        front = glm::normalize(front);
        
        glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, front));
        
        float velocity = m_CameraSpeed * deltaTime;
        
        // Handle movement input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_CameraPosition += front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_CameraPosition -= front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_CameraPosition -= right * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_CameraPosition += right * velocity;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            m_CameraPosition += up * velocity;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            m_CameraPosition -= up * velocity;
    }
    
    /**
     * @brief Update camera view and projection matrices
     */
    void UpdateCameraMatrices() {
        // Calculate camera direction
        glm::vec3 front;
        front.x = cos(glm::radians(m_CameraRotation.x)) * cos(glm::radians(m_CameraRotation.y));
        front.y = sin(glm::radians(m_CameraRotation.y));
        front.z = sin(glm::radians(m_CameraRotation.x)) * cos(glm::radians(m_CameraRotation.y));
        front = glm::normalize(front);
        
        // Create view matrix
        glm::mat4 view = glm::lookAt(m_CameraPosition, m_CameraPosition + front, glm::vec3(0.0f, 1.0f, 0.0f));
        SilicaEngine::Renderer::SetViewMatrix(view);
        
        // Create projection matrix
        uint32_t width, height;
        GetWindow().GetSize(width, height);
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        glm::mat4 projection = glm::perspective(glm::radians(75.0f), aspect, 0.1f, 1000.0f);
        SilicaEngine::Renderer::SetProjectionMatrix(projection);
    }
    
    /**
     * @brief Update garden cube animations based on current mode
     */
    void UpdateGardenAnimation() {
        SE_PROFILE_FUNCTION();
        
        const float waveSpeed = 2.0f;
        const float pulseSpeed = 1.5f;
        const float orbitSpeed = 0.8f;
        const float chaosSpeed = 3.0f;
        
        // Clear cache periodically to prevent memory growth
        if (m_Time - m_LastCacheTime > CACHE_DURATION) {
            m_NoiseCache.clear();
            m_LastCacheTime = m_Time;
        }
        
        for (auto& cube : m_GardenCubes) {
            glm::vec3 offset(0.0f);
            float scaleMod = 1.0f;
            float colorMod = 1.0f;
            
            switch (m_CurrentMode) {
                case AnimationMode::Wave: {
                    // Sine wave ripples
                    float waveX = sin(m_Time * waveSpeed + cube.basePosition.x * 0.5f + cube.animationOffset);
                    float waveZ = sin(m_Time * waveSpeed * 0.7f + cube.basePosition.z * 0.3f + cube.animationOffset);
                    offset.y = (waveX + waveZ) * 0.5f;
                    scaleMod = 1.0f + sin(m_Time * waveSpeed + cube.animationOffset) * 0.2f;
                    colorMod = 1.0f + sin(m_Time * waveSpeed * 1.5f + cube.animationOffset) * 0.3f;
                    break;
                }
                
                case AnimationMode::Pulse: {
                    // Radial pulsing from center
                    float distance = glm::length(glm::vec2(cube.basePosition.x, cube.basePosition.z));
                    float pulse = sin(m_Time * pulseSpeed - distance * 0.5f + cube.animationOffset);
                    offset.y = pulse * 2.0f;
                    scaleMod = 1.0f + pulse * 0.3f;
                    colorMod = 1.0f + pulse * 0.5f;
                    break;
                }
                
                case AnimationMode::Orbit: {
                    // Cubes orbit around invisible points
                    float orbitRadius = 1.0f + sin(cube.animationOffset) * 0.5f;
                    float orbitAngle = m_Time * orbitSpeed + cube.animationOffset;
                    offset.x = cos(orbitAngle) * orbitRadius;
                    offset.z = sin(orbitAngle) * orbitRadius;
                    scaleMod = 1.0f + sin(orbitAngle * 2.0f) * 0.2f;
                    colorMod = 1.0f + sin(orbitAngle * 3.0f) * 0.4f;
                    break;
                }
                
                case AnimationMode::Chaos: {
                    SE_PROFILE_SCOPE("ChaosAnimation");
                    
                    // Random chaotic movement using noise with caching
                    
                    // Generate cache keys for this cube's noise values
                    std::string keyX = "x_" + std::to_string(static_cast<int>(cube.basePosition.x * 10)) + "_" + std::to_string(static_cast<int>(m_Time * 10));
                    std::string keyY = "y_" + std::to_string(static_cast<int>(cube.basePosition.y * 10)) + "_" + std::to_string(static_cast<int>(m_Time * 10));
                    std::string keyZ = "z_" + std::to_string(static_cast<int>(cube.basePosition.z * 10)) + "_" + std::to_string(static_cast<int>(m_Time * 10));
                    std::string keyScale = "scale_" + std::to_string(static_cast<int>(cube.basePosition.x * 5)) + "_" + std::to_string(static_cast<int>(m_Time * 10));
                    std::string keyColor = "color_" + std::to_string(static_cast<int>(cube.basePosition.x * 5)) + "_" + std::to_string(static_cast<int>(m_Time * 10));
                    
                    // Get or compute noise values with caching
                    auto getCachedNoise = [this](const std::string& key, const glm::vec3& pos, float time, float speed, float offset) -> float {
                        auto it = m_NoiseCache.find(key);
                        if (it != m_NoiseCache.end()) {
                            return it->second;
                        }
                        float noise = glm::simplex(glm::vec3(pos.x * 0.1f, time * speed, offset));
                        m_NoiseCache[key] = noise;
                        return noise;
                    };
                    
                    auto getCachedNoise4D = [this](const std::string& key, const glm::vec3& pos, float time, float speed) -> float {
                        auto it = m_NoiseCache.find(key);
                        if (it != m_NoiseCache.end()) {
                            return it->second;
                        }
                        float noise = glm::simplex(glm::vec4(pos * 0.1f, time * speed));
                        m_NoiseCache[key] = noise;
                        return noise;
                    };
                    
                    offset.x = getCachedNoise(keyX, cube.basePosition, m_Time, chaosSpeed, 0.0f) * 3.0f;
                    offset.y = getCachedNoise(keyY, cube.basePosition, m_Time, chaosSpeed, 100.0f) * 2.0f;
                    offset.z = getCachedNoise(keyZ, cube.basePosition, m_Time, chaosSpeed, 200.0f) * 3.0f;
                    scaleMod = 1.0f + getCachedNoise4D(keyScale, cube.basePosition, m_Time, chaosSpeed) * 0.5f;
                    colorMod = 1.0f + getCachedNoise4D(keyColor, cube.basePosition * 0.5f, m_Time, chaosSpeed) * 0.8f;
                    break;
                }
            }
            
            // Apply transformations
            cube.currentPosition = cube.basePosition + offset;
            cube.scale = std::max(0.1f, scaleMod);
            
            // Apply color modulation with glow effect
            cube.currentColor = cube.baseColor * std::max(0.3f, colorMod);
            cube.currentColor.w = 1.0f; // Maintain alpha
        }
    }
    
    /**
     * @brief Get atmospheric background color that changes over time
     */
    glm::vec3 GetAtmosphericColor() {
        float r = 0.1f + 0.05f * sin(m_Time * 0.3f);
        float g = 0.15f + 0.1f * sin(m_Time * 0.2f + 2.0f);
        float b = 0.25f + 0.15f * sin(m_Time * 0.1f + 4.0f);
        return glm::vec3(r, g, b);
    }
    
    /**
     * @brief Convert HSV to RGB color space
     */
    glm::vec4 HSVtoRGB(float h, float s, float v) {
        float c = v * s;
        float x = c * (1.0f - abs(fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = v - c;
        
        glm::vec3 rgb;
        if (h >= 0 && h < 60) {
            rgb = glm::vec3(c, x, 0);
        } else if (h >= 60 && h < 120) {
            rgb = glm::vec3(x, c, 0);
        } else if (h >= 120 && h < 180) {
            rgb = glm::vec3(0, c, x);
        } else if (h >= 180 && h < 240) {
            rgb = glm::vec3(0, x, c);
        } else if (h >= 240 && h < 300) {
            rgb = glm::vec3(x, 0, c);
        } else {
            rgb = glm::vec3(c, 0, x);
        }
        
        rgb += m;
        return glm::vec4(rgb, 1.0f);
    }
    
    /**
     * @brief Draw performance statistics and information overlay
     */
    void DrawStatistics() {
        static float statsTimer = 0.0f;
        statsTimer += GetDeltaTime();
        
        if (statsTimer >= 1.0f) {
            const auto& stats = SilicaEngine::Renderer::GetStats();
            
            SE_APP_TRACE("=== Shatter Garden Statistics ===");
            SE_APP_TRACE("FPS: {:.1f}", GetFPS());
            SE_APP_TRACE("Frame Time: {:.2f}ms", GetDeltaTime() * 1000.0f);
            SE_APP_TRACE("Draw Calls: {}", stats.drawCalls);
            SE_APP_TRACE("Vertices: {}", stats.vertices);
            SE_APP_TRACE("Triangles: {}", stats.triangles);
            SE_APP_TRACE("Garden Cubes: {}", m_GardenCubes.size());
            SE_APP_TRACE("Camera Position: ({:.1f}, {:.1f}, {:.1f})", 
                         m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
            SE_APP_TRACE("Animation Mode: {}", 
                         m_CurrentMode == AnimationMode::Wave ? "WAVE" :
                         m_CurrentMode == AnimationMode::Pulse ? "PULSE" :
                         m_CurrentMode == AnimationMode::Orbit ? "ORBIT" : "CHAOS");
            SE_APP_TRACE("Time: {:.1f}s {}", m_Time, m_TimeFrozen ? "[FROZEN]" : "");
            
            statsTimer = 0.0f;
        }
    }
};

/**
 * @brief Main entry point
 */
int main() {
    try {
        // Create and run the garden
        FracturaShatterGarden garden;
        return garden.Run();
    }
    catch (const std::exception& e) {
        SE_APP_CRITICAL("Unhandled exception: {}", e.what());
        return -1;
    }
    catch (...) {
        SE_APP_CRITICAL("Unknown exception occurred");
        return -1;
    }
}