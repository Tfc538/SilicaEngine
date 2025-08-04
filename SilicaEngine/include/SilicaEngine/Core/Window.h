/**
 * @file Window.h
 * @brief Window management class for SilicaEngine using GLFW
 * @version 1.0.0
 * 
 * Provides cross-platform window creation and management functionality
 * with OpenGL context support.
 */

#pragma once

#include <glad/gl.h>           // Must be included before GLFW
#define GLFW_INCLUDE_NONE      // Prevent GLFW from including OpenGL headers
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

namespace SilicaEngine {

    /**
     * @struct WindowProperties
     * @brief Properties for window creation
     */
    struct WindowProperties {
        std::string title = "SilicaEngine Window";  ///< Window title
        uint32_t width = 1280;                      ///< Window width
        uint32_t height = 720;                      ///< Window height
        bool resizable = true;                      ///< Is window resizable
        bool decorated = true;                      ///< Window decorations (title bar, etc.)
        bool focused = true;                        ///< Focus window on creation
        bool maximized = false;                     ///< Start maximized
        bool visible = true;                        ///< Window visibility
        bool transparent = false;                   ///< Transparent framebuffer
        int samples = 0;                           ///< MSAA samples (0 = disabled)
    };

    /**
     * @struct OpenGLProperties
     * @brief OpenGL context properties
     */
    struct OpenGLProperties {
        int majorVersion = 4;                       ///< OpenGL major version
        int minorVersion = 6;                       ///< OpenGL minor version
        bool coreProfile = true;                    ///< Use core profile
        bool forwardCompatible = true;              ///< Forward compatible context
        bool debugContext = false;                  ///< Debug context
        int colorBits = 24;                        ///< Color buffer bits
        int depthBits = 24;                        ///< Depth buffer bits
        int stencilBits = 8;                       ///< Stencil buffer bits
    };

    /**
     * @class Window
     * @brief Cross-platform window wrapper using GLFW
     * 
     * Manages window creation, OpenGL context setup, and provides
     * a clean interface for window operations and event handling.
     */
    class Window {
    public:
        /**
         * @brief Constructor
         * @param windowProps Window properties
         * @param openglProps OpenGL context properties
         */
        Window(const WindowProperties& windowProps = WindowProperties{},
               const OpenGLProperties& openglProps = OpenGLProperties{});

        /**
         * @brief Destructor
         */
        ~Window();

        /**
         * @brief Initialize the window and OpenGL context
         * @return true if initialization was successful
         */
        bool Initialize();

        /**
         * @brief Shutdown and cleanup the window
         */
        void Shutdown();

        /**
         * @brief Poll window events
         */
        void PollEvents();

        /**
         * @brief Swap front and back buffers
         */
        void SwapBuffers();

        /**
         * @brief Check if the window should close
         * @return true if the window should close
         */
        bool ShouldClose() const;

        /**
         * @brief Set whether the window should close
         * @param shouldClose Close flag
         */
        void SetShouldClose(bool shouldClose);

        /**
         * @brief Make the OpenGL context current
         */
        void MakeContextCurrent();

        /**
         * @brief Get the GLFW window handle
         * @return GLFW window pointer
         */
        GLFWwindow* GetNativeWindow() const;

        /**
         * @brief Get window width
         * @return Window width in pixels
         */
        uint32_t GetWidth() const;

        /**
         * @brief Get window height
         * @return Window height in pixels
         */
        uint32_t GetHeight() const;

        /**
         * @brief Get window size
         * @param width Reference to store width
         * @param height Reference to store height
         */
        void GetSize(uint32_t& width, uint32_t& height) const;

        /**
         * @brief Set window size
         * @param width New width
         * @param height New height
         */
        void SetSize(uint32_t width, uint32_t height);

        /**
         * @brief Get window position
         * @param x Reference to store X position
         * @param y Reference to store Y position
         */
        void GetPosition(int& x, int& y) const;

        /**
         * @brief Set window position
         * @param x New X position
         * @param y New Y position
         */
        void SetPosition(int x, int y);

        /**
         * @brief Get window title
         * @return Current window title
         */
        const std::string& GetTitle() const;

        /**
         * @brief Set window title
         * @param title New window title
         */
        void SetTitle(const std::string& title);

        /**
         * @brief Check if the window is focused
         * @return true if focused
         */
        bool IsFocused() const;

        /**
         * @brief Check if the window is minimized
         * @return true if minimized
         */
        bool IsMinimized() const;

        /**
         * @brief Check if the window is maximized
         * @return true if maximized
         */
        bool IsMaximized() const;

        /**
         * @brief Set window icon
         * @param width Icon width
         * @param height Icon height
         * @param pixels RGBA pixel data
         */
        void SetIcon(int width, int height, const unsigned char* pixels);

        /**
         * @brief Enable or disable VSync
         * @param enabled true to enable VSync
         */
        void SetVSync(bool enabled);

        /**
         * @brief Check if VSync is enabled
         * @return true if VSync is enabled
         */
        bool IsVSyncEnabled() const;

        /**
         * @brief Set the cursor mode
         * @param mode GLFW cursor mode (GLFW_CURSOR_NORMAL, GLFW_CURSOR_HIDDEN, GLFW_CURSOR_DISABLED)
         */
        void SetCursorMode(int mode);

        /**
         * @brief Get the cursor position
         * @param x Reference to store X position
         * @param y Reference to store Y position
         */
        void GetCursorPos(double& x, double& y) const;

        /**
         * @brief Set the cursor position
         * @param x New X position
         * @param y New Y position
         */
        void SetCursorPos(double x, double y);

        /**
         * @brief Set the user pointer for callbacks
         * @param pointer User pointer
         */
        void SetUserPointer(void* pointer);

        /**
         * @brief Get the user pointer
         * @return User pointer
         */
        void* GetUserPointer() const;

        /**
         * @brief Check if the window is initialized
         * @return true if initialized
         */
        bool IsInitialized() const;

        /**
         * @brief Get the window properties
         * @return Reference to window properties
         */
        const WindowProperties& GetWindowProperties() const;

        /**
         * @brief Get the OpenGL properties  
         * @return Reference to OpenGL properties
         */
        const OpenGLProperties& GetOpenGLProperties() const;

        /**
         * @brief Create a screenshot of the window
         * @param filename File to save the screenshot to
         * @return true if screenshot was saved successfully
         */
        bool SaveScreenshot(const std::string& filename) const;

    private:
        GLFWwindow* m_Window;                       ///< GLFW window handle
        WindowProperties m_WindowProps;             ///< Window properties
        OpenGLProperties m_OpenGLProps;             ///< OpenGL properties
        bool m_Initialized;                        ///< Initialization state
        bool m_VSyncEnabled;                       ///< VSync state

        /**
         * @brief Configure window hints for GLFW
         */
        void ConfigureWindowHints();

        /**
         * @brief Initialize OpenGL function pointers
         * @return true if successful
         */
        bool InitializeOpenGL();

        /**
         * @brief Set up default OpenGL state
         */
        void SetupOpenGLDefaults();

        /**
         * @brief Update window properties after creation
         */
        void UpdateWindowProperties();
    };

} // namespace SilicaEngine