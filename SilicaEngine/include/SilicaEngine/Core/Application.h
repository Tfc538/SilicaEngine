/**
 * @file Application.h
 * @brief Core application class for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Main Application class that handles lifecycle, window management, and game loop.
 */

#pragma once

#include "Window.h"
#include "SilicaEngine/Core/ErrorCodes.h"
#include "SilicaEngine/Debug/Profiler.h"
#include <memory>
#include <string>

namespace SilicaEngine {

    /// Configuration for application initialization
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

    /// Main application base class. Inherit and override virtual methods.
    class Application {
    public:
        explicit Application(const ApplicationConfig& config = ApplicationConfig{});
        virtual ~Application();

        /// Run the main application loop
        int Run();

        /// Request graceful shutdown
        void Close();

        Window& GetWindow() const;
        bool IsRunning() const;
        float GetTime() const;
        float GetDeltaTime() const;
        float GetFPS() const;

    protected:
        /// Override for custom initialization after window/OpenGL creation
        virtual ErrorResult<void> OnInitialize();

        /// Override for per-frame logic updates
        virtual void OnUpdate(float deltaTime);

        /// Override for rendering logic
        virtual void OnRender();
        
        /// Override for debug rendering (called after main render)
        virtual void OnDebugRender();

        /// Override for cleanup before shutdown
        virtual void OnShutdown();

        /// Event callbacks - override as needed
        virtual void OnWindowResize(uint32_t width, uint32_t height);
        virtual void OnKeyEvent(int key, int scancode, int action, int mods);
        virtual void OnMouseButtonEvent(int button, int action, int mods);
        virtual void OnMouseMoveEvent(double xpos, double ypos);
        virtual void OnScrollEvent(double xoffset, double yoffset);

    private:
        std::unique_ptr<Window> m_Window;
        ApplicationConfig m_Config;
        bool m_Running;
        bool m_Initialized;
        
        // Timing
        float m_LastFrameTime;
        float m_DeltaTime;
        float m_FrameCount;
        float m_FPSTimer;
        float m_FPS;

        ErrorResult<void> Initialize();
        ErrorResult<void> Shutdown();
        void UpdateTiming();
        void SetupWindowCallbacks();

        // GLFW callbacks
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    };

} // namespace SilicaEngine