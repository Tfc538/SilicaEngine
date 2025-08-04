/**
 * @file Window.h
 * @brief Cross-platform window management using GLFW
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.0.0
 * 
 * Window creation, OpenGL context setup, and event handling.
 */

#pragma once

#include <glad/gl.h>           // Must be included before GLFW
#define GLFW_INCLUDE_NONE      // Prevent GLFW from including OpenGL headers
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

namespace SilicaEngine {

    /// Window creation properties
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

    /// OpenGL context configuration
    struct OpenGLProperties {
        int majorVersion = 4;
        int minorVersion = 6;
        bool coreProfile = true;
        bool forwardCompatible = true;
        bool debugContext = false;
        int colorBits = 24;
        int depthBits = 24;
        int stencilBits = 8;
    };

    /// Cross-platform window wrapper with OpenGL context management
    class Window {
    public:
        Window(const WindowProperties& windowProps = WindowProperties{},
               const OpenGLProperties& openglProps = OpenGLProperties{});
        ~Window();

        /// Initialize window and OpenGL context
        bool Initialize();

        /// Cleanup and shutdown
        void Shutdown();

        /// Poll for window events
        void PollEvents();

        /// Swap front/back buffers
        void SwapBuffers();

        /// Check if window should close
        bool ShouldClose() const;

        /// Set window close flag
        void SetShouldClose(bool shouldClose);

        /// Make OpenGL context current
        void MakeContextCurrent();

        /// Get native GLFW window handle
        GLFWwindow* GetNativeWindow() const;

        // Window properties
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        void GetSize(uint32_t& width, uint32_t& height) const;
        void SetSize(uint32_t width, uint32_t height);
        void GetPosition(int& x, int& y) const;
        void SetPosition(int x, int y);
        const std::string& GetTitle() const;
        void SetTitle(const std::string& title);

        // Window state
        bool IsFocused() const;
        bool IsMinimized() const;
        bool IsMaximized() const;
        void SetIcon(int width, int height, const unsigned char* pixels);
        void SetVSync(bool enabled);
        bool IsVSyncEnabled() const;

        // Input and utilities
        void SetCursorMode(int mode); // GLFW_CURSOR_NORMAL/HIDDEN/DISABLED
        void GetCursorPos(double& x, double& y) const;
        void SetCursorPos(double x, double y);
        void SetUserPointer(void* pointer);
        void* GetUserPointer() const;
        bool IsInitialized() const;
        const WindowProperties& GetWindowProperties() const;
        const OpenGLProperties& GetOpenGLProperties() const;
        bool SaveScreenshot(const std::string& filename) const;

    private:
        GLFWwindow* m_Window;
        WindowProperties m_WindowProps;
        OpenGLProperties m_OpenGLProps;
        bool m_Initialized;
        bool m_VSyncEnabled;

        void ConfigureWindowHints();
        bool InitializeOpenGL();
        void SetupOpenGLDefaults();
        void UpdateWindowProperties();
    };

} // namespace SilicaEngine