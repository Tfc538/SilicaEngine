/**
 * @file main.cpp
 * @brief Main entry point for Fractura - Example game using SilicaEngine
 * @version 1.0.0
 * 
 * Fractura demonstrates the capabilities of the SilicaEngine by creating
 * a simple 3D scene with rotating cubes, camera controls, and basic lighting.
 */

#include <SilicaEngine/SilicaEngine.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>

/**
 * @class FracturaApp
 * @brief Main application class for Fractura
 * 
 * Demonstrates engine features including:
 * - 3D rendering with multiple objects
 * - Camera controls (WASD movement, mouse look)
 * - Primitive shapes (cubes, grid)
 * - Basic animation (rotation)
 * - Real-time statistics display
 */
class FracturaApp : public SilicaEngine::Application {
public:
    FracturaApp() 
        : SilicaEngine::Application(GetFracturaConfig())
        , m_CameraPosition(0.0f, 2.0f, 5.0f)
        , m_CameraRotation(0.0f, 0.0f)
        , m_CameraSpeed(5.0f)
        , m_MouseSensitivity(0.1f)
        , m_Time(0.0f)
        , m_LastMouseX(0.0)
        , m_LastMouseY(0.0)
        , m_FirstMouse(true)
        , m_WireframeMode(false)
        , m_ShowStats(true) {
        
        // Initialize cube positions
        m_CubePositions = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };
        
        // Initialize cube colors
        m_CubeColors = {
            glm::vec4(1.0f, 0.5f, 0.2f, 1.0f),  // Orange
            glm::vec4(0.2f, 1.0f, 0.5f, 1.0f),  // Green
            glm::vec4(0.5f, 0.2f, 1.0f, 1.0f),  // Purple
            glm::vec4(1.0f, 1.0f, 0.2f, 1.0f),  // Yellow
            glm::vec4(0.2f, 0.5f, 1.0f, 1.0f),  // Blue
            glm::vec4(1.0f, 0.2f, 0.5f, 1.0f),  // Pink
            glm::vec4(0.5f, 1.0f, 0.2f, 1.0f),  // Lime
            glm::vec4(1.0f, 0.2f, 1.0f, 1.0f),  // Magenta
            glm::vec4(0.2f, 1.0f, 1.0f, 1.0f),  // Cyan
            glm::vec4(1.0f, 0.5f, 0.5f, 1.0f)   // Light Red
        };
    }

protected:
    bool OnInitialize() override {
        SE_APP_INFO("Initializing Fractura...");
        
        // Setup camera
        UpdateCameraMatrices();
        
        // Set mouse cursor to center and capture it
        GetWindow().SetCursorMode(GLFW_CURSOR_DISABLED);
        
        // Get initial mouse position
        GetWindow().GetCursorPos(m_LastMouseX, m_LastMouseY);
        
        SE_APP_INFO("Fractura initialized successfully!");
        SE_APP_INFO("Controls:");
        SE_APP_INFO("  WASD - Move camera");
        SE_APP_INFO("  Mouse - Look around");
        SE_APP_INFO("  F1 - Toggle wireframe mode");
        SE_APP_INFO("  F2 - Toggle statistics");
        SE_APP_INFO("  ESC - Exit");
        
        return true;
    }
    
    void OnUpdate(float deltaTime) override {
        m_Time += deltaTime;
        
        // Handle input
        HandleInput(deltaTime);
        
        // Update camera matrices
        UpdateCameraMatrices();
    }
    
    void OnRender() override {
        // Clear screen
        SilicaEngine::Renderer::SetClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        SilicaEngine::Renderer::Clear();
        
        // Draw grid
        SilicaEngine::Renderer::DrawGrid(20.0f, 20, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
        
        // Draw cubes with scaling animation
        for (size_t i = 0; i < m_CubePositions.size(); ++i) {
            float scaleAmount = 0.8f + 0.4f * sin(m_Time * 2.0f + static_cast<float>(i));
            
            glm::vec3 position = m_CubePositions[i];
            glm::vec3 size = glm::vec3(scaleAmount);
            glm::vec4 color = m_CubeColors[i];
            
            // Add some vertical movement
            position.y += sin(m_Time + static_cast<float>(i)) * 0.5f;
            
            SilicaEngine::Renderer::DrawCube(position, size, color);
        }
        
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
                case GLFW_KEY_F1:
                    m_WireframeMode = !m_WireframeMode;
                    SilicaEngine::Renderer::SetWireframe(m_WireframeMode);
                    SE_APP_INFO("Wireframe mode: {}", m_WireframeMode ? "ON" : "OFF");
                    break;
                    
                case GLFW_KEY_F2:
                    m_ShowStats = !m_ShowStats;
                    SE_APP_INFO("Statistics display: {}", m_ShowStats ? "ON" : "OFF");
                    break;
                    
                case GLFW_KEY_R:
                    // Reset camera position
                    m_CameraPosition = glm::vec3(0.0f, 2.0f, 5.0f);
                    m_CameraRotation = glm::vec2(0.0f);
                    SE_APP_INFO("Camera reset");
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
    
    // Animation
    float m_Time;
    
    // Mouse handling
    double m_LastMouseX, m_LastMouseY;
    bool m_FirstMouse;
    
    // Scene objects
    std::vector<glm::vec3> m_CubePositions;
    std::vector<glm::vec4> m_CubeColors;
    
    // Rendering options
    bool m_WireframeMode;
    bool m_ShowStats;
    
    /**
     * @brief Get application configuration for Fractura
     */
    static SilicaEngine::ApplicationConfig GetFracturaConfig() {
        SilicaEngine::ApplicationConfig config;
        config.windowTitle = "Fractura - SilicaEngine Demo";
        config.windowWidth = 1280;
        config.windowHeight = 720;
        config.windowResizable = true;
        config.enableVSync = true;
        config.openglMajorVersion = 4;
        config.openglMinorVersion = 6;
        return config;
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
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
        SilicaEngine::Renderer::SetProjectionMatrix(projection);
    }
    
    /**
     * @brief Draw performance statistics and information overlay
     */
    void DrawStatistics() {
        // This is a placeholder for text rendering
        // In a real implementation, you'd use a text rendering system
        // For now, we'll just log the statistics periodically
        
        static float statsTimer = 0.0f;
        statsTimer += GetDeltaTime();
        
        if (statsTimer >= 1.0f) { // Update every second
            const auto& stats = SilicaEngine::Renderer::GetStats();
            
            SE_APP_TRACE("=== Performance Statistics ===");
            SE_APP_TRACE("FPS: {:.1f}", GetFPS());
            SE_APP_TRACE("Frame Time: {:.2f}ms", GetDeltaTime() * 1000.0f);
            SE_APP_TRACE("Draw Calls: {}", stats.drawCalls);
            SE_APP_TRACE("Vertices: {}", stats.vertices);
            SE_APP_TRACE("Triangles: {}", stats.triangles);
            SE_APP_TRACE("Camera Position: ({:.1f}, {:.1f}, {:.1f})", 
                         m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
            SE_APP_TRACE("Camera Rotation: ({:.1f}, {:.1f})", 
                         m_CameraRotation.x, m_CameraRotation.y);
            
            statsTimer = 0.0f;
        }
    }
};

/**
 * @brief Main entry point
 */
int main() {
    try {
        // Create and run the application
        FracturaApp app;
        return app.Run();
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