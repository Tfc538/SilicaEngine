/**
 * @file Application.cpp
 * @brief Implementation of the Application class for SilicaEngine
 */

#include "SilicaEngine/Core/Application.h"
#include "SilicaEngine/Core/Logger.h"
#include "SilicaEngine/Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace SilicaEngine {

    Application::Application(const ApplicationConfig& config)
        : m_Config(config)
        , m_Running(false)
        , m_Initialized(false)
        , m_LastFrameTime(0.0f)
        , m_DeltaTime(0.0f)
        , m_FrameCount(0.0f)
        , m_FPSTimer(0.0f)
        , m_FPS(0.0f) {
        
        SE_INFO("Application created with config:");
        SE_INFO("  Title: {}", m_Config.windowTitle);
        SE_INFO("  Size: {}x{}", m_Config.windowWidth, m_Config.windowHeight);
        SE_INFO("  OpenGL: {}.{}", m_Config.openglMajorVersion, m_Config.openglMinorVersion);
    }

    Application::~Application() {
        if (m_Initialized) {
            Shutdown();
        }
    }

    int Application::Run() {
        if (!Initialize()) {
            SE_ERROR("Failed to initialize application");
            return -1;
        }

        SE_INFO("Starting main application loop");
        m_Running = true;
        m_LastFrameTime = static_cast<float>(glfwGetTime());

        // Main loop
        while (m_Running && !m_Window->ShouldClose()) {
            // Update timing
            UpdateTiming();

            // Poll events
            m_Window->PollEvents();

            // Update application
            OnUpdate(m_DeltaTime);

            // Render
            Renderer::BeginFrame();
            OnRender();
            Renderer::EndFrame();

            // Swap buffers
            m_Window->SwapBuffers();
        }

        SE_INFO("Application loop ended");
        Shutdown();
        return 0;
    }

    void Application::Close() {
        SE_INFO("Application close requested");
        m_Running = false;
    }

    Window& Application::GetWindow() const {
        return *m_Window;
    }

    bool Application::IsRunning() const {
        return m_Running;
    }

    float Application::GetTime() const {
        return static_cast<float>(glfwGetTime());
    }

    float Application::GetDeltaTime() const {
        return m_DeltaTime;
    }

    float Application::GetFPS() const {
        return m_FPS;
    }

    bool Application::OnInitialize() {
        SE_INFO("Default application initialization");
        return true;
    }

    void Application::OnUpdate(float deltaTime) {
        // Default implementation does nothing
        (void)deltaTime; // Suppress unused parameter warning
    }

    void Application::OnRender() {
        // Default implementation - clear screen with dark gray
        Renderer::SetClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        Renderer::Clear();
    }

    void Application::OnShutdown() {
        SE_INFO("Default application shutdown");
    }

    void Application::OnWindowResize(uint32_t width, uint32_t height) {
        SE_INFO("Window resized to {}x{}", width, height);
        
        // Update viewport
        Renderer::SetViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
    }

    void Application::OnKeyEvent(int key, int scancode, int action, int mods) {
        // Default implementation - log key events
        (void)scancode; (void)mods; // Suppress unused parameter warnings
        
        if (action == GLFW_PRESS) {
            SE_TRACE("Key pressed: {}", key);
            
            // ESC to close application
            if (key == GLFW_KEY_ESCAPE) {
                Close();
            }
        }
    }

    void Application::OnMouseButtonEvent(int button, int action, int mods) {
        // Default implementation - log mouse events
        (void)button; (void)action; (void)mods; // Suppress unused parameter warnings
        SE_TRACE("Mouse button event: {} {}", button, action);
    }

    void Application::OnMouseMoveEvent(double xpos, double ypos) {
        // Default implementation - don't log to avoid spam
        (void)xpos; (void)ypos; // Suppress unused parameter warnings
    }

    void Application::OnScrollEvent(double xoffset, double yoffset) {
        // Default implementation - log scroll events
        SE_TRACE("Scroll event: {}, {}", xoffset, yoffset);
    }

    bool Application::Initialize() {
        SE_INFO("Initializing application...");

        // Initialize logger first
        Logger::Initialize();

        // Create window properties
        WindowProperties windowProps;
        windowProps.title = m_Config.windowTitle;
        windowProps.width = m_Config.windowWidth;
        windowProps.height = m_Config.windowHeight;
        windowProps.resizable = m_Config.windowResizable;

        // Create OpenGL properties
        OpenGLProperties openglProps;
        openglProps.majorVersion = m_Config.openglMajorVersion;
        openglProps.minorVersion = m_Config.openglMinorVersion;
        openglProps.debugContext = true; // Enable debug context in debug builds

        // Create window
        m_Window = std::make_unique<Window>(windowProps, openglProps);
        if (!m_Window->Initialize()) {
            SE_ERROR("Failed to initialize window");
            return false;
        }

        // Set window user pointer for callbacks
        m_Window->SetUserPointer(this);

        // Setup window callbacks
        SetupWindowCallbacks();

        // Initialize renderer
        if (!Renderer::Initialize()) {
            SE_ERROR("Failed to initialize renderer");
            return false;
        }

        // Set VSync based on config
        m_Window->SetVSync(m_Config.enableVSync);

        // Set initial viewport
        Renderer::SetViewport(0, 0, static_cast<int>(m_Config.windowWidth), static_cast<int>(m_Config.windowHeight));

        // Call user initialization
        if (!OnInitialize()) {
            SE_ERROR("Application-specific initialization failed");
            return false;
        }

        m_Initialized = true;
        SE_INFO("Application initialized successfully");
        return true;
    }

    void Application::Shutdown() {
        if (!m_Initialized) {
            return;
        }

        SE_INFO("Shutting down application...");

        // Call user shutdown
        OnShutdown();

        // Shutdown renderer
        Renderer::Shutdown();

        // Shutdown window
        if (m_Window) {
            m_Window->Shutdown();
            m_Window.reset();
        }

        // Shutdown logger last
        Logger::Shutdown();

        m_Initialized = false;
        m_Running = false;
    }

    void Application::UpdateTiming() {
        float currentTime = GetTime();
        m_DeltaTime = currentTime - m_LastFrameTime;
        m_LastFrameTime = currentTime;

        // Update FPS calculation
        m_FrameCount++;
        m_FPSTimer += m_DeltaTime;

        if (m_FPSTimer >= 1.0f) {
            m_FPS = m_FrameCount / m_FPSTimer;
            m_FrameCount = 0.0f;
            m_FPSTimer = 0.0f;
        }
    }

    void Application::SetupWindowCallbacks() {
        if (!m_Window || !m_Window->GetNativeWindow()) {
            return;
        }

        GLFWwindow* window = m_Window->GetNativeWindow();

        // Window resize callback
        glfwSetWindowSizeCallback(window, WindowResizeCallback);

        // Key callback
        glfwSetKeyCallback(window, KeyCallback);

        // Mouse button callback
        glfwSetMouseButtonCallback(window, MouseButtonCallback);

        // Cursor position callback
        glfwSetCursorPosCallback(window, CursorPosCallback);

        // Scroll callback
        glfwSetScrollCallback(window, ScrollCallback);
    }

    // Static callback functions
    void Application::WindowResizeCallback(GLFWwindow* window, int width, int height) {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app) {
            app->OnWindowResize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }
    }

    void Application::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app) {
            app->OnKeyEvent(key, scancode, action, mods);
        }
    }

    void Application::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app) {
            app->OnMouseButtonEvent(button, action, mods);
        }
    }

    void Application::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app) {
            app->OnMouseMoveEvent(xpos, ypos);
        }
    }

    void Application::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app) {
            app->OnScrollEvent(xoffset, yoffset);
        }
    }

} // namespace SilicaEngine