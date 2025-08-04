/**
 * @file Application.h
 * @brief Core application class for SilicaEngine
 * @version 1.0.0
 * 
 * Defines the main Application class that handles the application lifecycle,
 * window management, and the main game loop.
 */

#pragma once

#include "Window.h"
#include <memory>
#include <string>

namespace SilicaEngine {

    /**
     * @struct ApplicationConfig
     * @brief Configuration structure for application initialization
     */
    struct ApplicationConfig {
        std::string windowTitle = "SilicaEngine Application";  ///< Window title
        uint32_t windowWidth = 1280;                           ///< Initial window width
        uint32_t windowHeight = 720;                           ///< Initial window height
        bool windowResizable = true;                           ///< Whether window is resizable
        bool enableVSync = true;                               ///< Enable vertical synchronization
        bool fullscreen = false;                               ///< Start in fullscreen mode
        int openglMajorVersion = 4;                           ///< Required OpenGL major version
        int openglMinorVersion = 6;                           ///< Required OpenGL minor version
    };

    /**
     * @class Application
     * @brief Main application class for SilicaEngine applications
     * 
     * This class provides the foundation for all SilicaEngine applications.
     * It manages the application lifecycle, window creation, and the main
     * game loop. Applications should inherit from this class and override
     * the virtual methods to implement custom behavior.
     */
    class Application {
    public:
        /**
         * @brief Constructor
         * @param config Application configuration
         */
        explicit Application(const ApplicationConfig& config = ApplicationConfig{});

        /**
         * @brief Virtual destructor
         */
        virtual ~Application();

        /**
         * @brief Run the application
         * @return Exit code (0 for success)
         * 
         * Starts the main application loop and handles initialization,
         * update/render cycles, and cleanup.
         */
        int Run();

        /**
         * @brief Request application shutdown
         * 
         * Sets a flag to exit the main loop gracefully on the next frame.
         */
        void Close();

        /**
         * @brief Get the application window
         * @return Reference to the window instance
         */
        Window& GetWindow() const;

        /**
         * @brief Check if the application is running
         * @return true if running, false if shutdown requested
         */
        bool IsRunning() const;

        /**
         * @brief Get the time since application start in seconds
         * @return Time in seconds as a float
         */
        float GetTime() const;

        /**
         * @brief Get the delta time since last frame in seconds
         * @return Delta time in seconds as a float
         */
        float GetDeltaTime() const;

        /**
         * @brief Get current frames per second
         * @return FPS as a float
         */
        float GetFPS() const;

    protected:
        /**
         * @brief Called once during application initialization
         * @return true if initialization was successful
         * 
         * Override this method to perform custom initialization.
         * This is called after the window and OpenGL context are created.
         */
        virtual bool OnInitialize();

        /**
         * @brief Called every frame to update application logic
         * @param deltaTime Time since last frame in seconds
         * 
         * Override this method to implement game logic, input handling,
         * physics updates, etc.
         */
        virtual void OnUpdate(float deltaTime);

        /**
         * @brief Called every frame to render the scene
         * 
         * Override this method to implement rendering logic.
         * The OpenGL context is current when this is called.
         */
        virtual void OnRender();

        /**
         * @brief Called when the application is about to shutdown
         * 
         * Override this method to perform custom cleanup.
         * This is called before the OpenGL context is destroyed.
         */
        virtual void OnShutdown();

        /**
         * @brief Called when the window is resized
         * @param width New window width
         * @param height New window height
         */
        virtual void OnWindowResize(uint32_t width, uint32_t height);

        /**
         * @brief Called when a key is pressed, released, or repeated
         * @param key The keyboard key
         * @param scancode Platform-specific scancode
         * @param action GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT
         * @param mods Modifier keys
         */
        virtual void OnKeyEvent(int key, int scancode, int action, int mods);

        /**
         * @brief Called when a mouse button is pressed or released
         * @param button The mouse button
         * @param action GLFW_PRESS or GLFW_RELEASE
         * @param mods Modifier keys
         */
        virtual void OnMouseButtonEvent(int button, int action, int mods);

        /**
         * @brief Called when the mouse cursor moves
         * @param xpos New X position
         * @param ypos New Y position
         */
        virtual void OnMouseMoveEvent(double xpos, double ypos);

        /**
         * @brief Called when the mouse wheel is scrolled
         * @param xoffset Horizontal scroll offset
         * @param yoffset Vertical scroll offset
         */
        virtual void OnScrollEvent(double xoffset, double yoffset);

    private:
        std::unique_ptr<Window> m_Window;           ///< Application window
        ApplicationConfig m_Config;                 ///< Application configuration
        bool m_Running;                            ///< Running state
        bool m_Initialized;                        ///< Initialization state
        
        // Timing
        float m_LastFrameTime;                     ///< Time of last frame
        float m_DeltaTime;                         ///< Time since last frame
        float m_FrameCount;                        ///< Frame counter for FPS
        float m_FPSTimer;                          ///< Timer for FPS calculation
        float m_FPS;                               ///< Current FPS

        /**
         * @brief Initialize the application
         * @return true if initialization was successful
         */
        bool Initialize();

        /**
         * @brief Shutdown the application
         */
        void Shutdown();

        /**
         * @brief Update timing information
         */
        void UpdateTiming();

        /**
         * @brief Setup window event callbacks
         */
        void SetupWindowCallbacks();

        // Event callback functions (static for GLFW compatibility)
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    };

} // namespace SilicaEngine