/**
 * @file Window.cpp
 * @brief Window management implementation
 * @author Tim Gatzke <post@tim-gatzke.de>
 */

#include "SilicaEngine/Core/Window.h"
#include "SilicaEngine/Core/Logger.h"
#include <iostream>
#include <fstream>

namespace SilicaEngine {

    Window::Window(const WindowProperties& windowProps, const OpenGLProperties& openglProps)
        : m_Window(nullptr)
        , m_WindowProps(windowProps)
        , m_OpenGLProps(openglProps)
        , m_Initialized(false)
        , m_VSyncEnabled(true) {
    }

    Window::~Window() {
        Shutdown();
    }

    ErrorResult<void> Window::Initialize() {
        if (m_Initialized) {
            return ErrorResult<void>::Success();
        }

        // Initialize GLFW
        if (!glfwInit()) {
            SE_ERROR("Failed to initialize GLFW");
            return ErrorResult<void>::Error(EngineError::InitializationFailed, "Failed to initialize GLFW");
        }

        SE_INFO("GLFW initialized successfully");

        // Configure window hints
        ConfigureWindowHints();

        // Create window
        m_Window = glfwCreateWindow(
            static_cast<int>(m_WindowProps.width),
            static_cast<int>(m_WindowProps.height),
            m_WindowProps.title.c_str(),
            nullptr,
            nullptr
        );

        if (!m_Window) {
            SE_ERROR("Failed to create GLFW window");
            glfwTerminate();
            return ErrorResult<void>::Error(EngineError::WindowCreationFailed, "Failed to create GLFW window");
        }

        SE_INFO("Window created: {}x{} - '{}'", m_WindowProps.width, m_WindowProps.height, m_WindowProps.title);

        // Make context current
        MakeContextCurrent();

        // Initialize OpenGL function pointers
        if (!InitializeOpenGL()) {
            SE_ERROR("Failed to initialize OpenGL");
            Shutdown();
            return ErrorResult<void>::Error(EngineError::InitializationFailed, "Failed to initialize OpenGL");
        }

        // Set up default OpenGL state
        SetupOpenGLDefaults();

        // Update window properties
        UpdateWindowProperties();

        // Set VSync
        SetVSync(true);

        // Set visibility
        if (m_WindowProps.visible) {
            glfwShowWindow(m_Window);
        } else {
            glfwHideWindow(m_Window);
        }

        m_Initialized = true;
        SE_INFO("Window initialization completed successfully");

        return ErrorResult<void>::Success();
    }

    ErrorResult<void> Window::Shutdown() {
        if (!m_Initialized) {
            return ErrorResult<void>::Success();
        }

        if (m_Window) {
            SE_INFO("Shutting down window");
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }

        glfwTerminate();
        m_Initialized = false;
        return ErrorResult<void>::Success();
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }

    void Window::SwapBuffers() {
        if (m_Window) {
            glfwSwapBuffers(m_Window);
        }
    }

    bool Window::ShouldClose() const {
        return m_Window ? glfwWindowShouldClose(m_Window) : true;
    }

    void Window::SetShouldClose(bool shouldClose) {
        if (m_Window) {
            glfwSetWindowShouldClose(m_Window, shouldClose ? GLFW_TRUE : GLFW_FALSE);
        }
    }

    void Window::MakeContextCurrent() {
        if (m_Window) {
            glfwMakeContextCurrent(m_Window);
        }
    }

    GLFWwindow* Window::GetNativeWindow() const {
        return m_Window;
    }

    uint32_t Window::GetWidth() const {
        return m_WindowProps.width;
    }

    uint32_t Window::GetHeight() const {
        return m_WindowProps.height;
    }

    void Window::GetSize(uint32_t& width, uint32_t& height) const {
        width = m_WindowProps.width;
        height = m_WindowProps.height;
    }

    void Window::SetSize(uint32_t width, uint32_t height) {
        if (m_Window) {
            glfwSetWindowSize(m_Window, static_cast<int>(width), static_cast<int>(height));
            m_WindowProps.width = width;
            m_WindowProps.height = height;
        }
    }

    void Window::GetPosition(int& x, int& y) const {
        if (m_Window) {
            glfwGetWindowPos(m_Window, &x, &y);
        }
    }

    void Window::SetPosition(int x, int y) {
        if (m_Window) {
            glfwSetWindowPos(m_Window, x, y);
        }
    }

    const std::string& Window::GetTitle() const {
        return m_WindowProps.title;
    }

    void Window::SetTitle(const std::string& title) {
        m_WindowProps.title = title;
        if (m_Window) {
            glfwSetWindowTitle(m_Window, title.c_str());
        }
    }

    bool Window::IsFocused() const {
        return m_Window ? glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) == GLFW_TRUE : false;
    }

    bool Window::IsMinimized() const {
        return m_Window ? glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) == GLFW_TRUE : false;
    }

    bool Window::IsMaximized() const {
        return m_Window ? glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) == GLFW_TRUE : false;
    }

    void Window::SetIcon(int width, int height, const unsigned char* pixels) {
        if (m_Window) {
            GLFWimage icon;
            icon.width = width;
            icon.height = height;
            icon.pixels = const_cast<unsigned char*>(pixels);
            glfwSetWindowIcon(m_Window, 1, &icon);
        }
    }

    void Window::SetVSync(bool enabled) {
        glfwSwapInterval(enabled ? 1 : 0);
        m_VSyncEnabled = enabled;
        SE_INFO("VSync {}", enabled ? "enabled" : "disabled");
    }

    bool Window::IsVSyncEnabled() const {
        return m_VSyncEnabled;
    }

    void Window::SetCursorMode(int mode) {
        if (m_Window) {
            glfwSetInputMode(m_Window, GLFW_CURSOR, mode);
        }
    }

    void Window::GetCursorPos(double& x, double& y) const {
        if (m_Window) {
            glfwGetCursorPos(m_Window, &x, &y);
        }
    }

    void Window::SetCursorPos(double x, double y) {
        if (m_Window) {
            glfwSetCursorPos(m_Window, x, y);
        }
    }

    void Window::SetUserPointer(void* pointer) {
        if (m_Window) {
            glfwSetWindowUserPointer(m_Window, pointer);
        }
    }

    void* Window::GetUserPointer() const {
        return m_Window ? glfwGetWindowUserPointer(m_Window) : nullptr;
    }

    bool Window::IsInitialized() const {
        return m_Initialized;
    }

    const WindowProperties& Window::GetWindowProperties() const {
        return m_WindowProps;
    }

    const OpenGLProperties& Window::GetOpenGLProperties() const {
        return m_OpenGLProps;
    }

    bool Window::SaveScreenshot(const std::string& filename) const {
        if (!m_Window) {
            return false;
        }

        // Get framebuffer size
        int width, height;
        glfwGetFramebufferSize(m_Window, &width, &height);

        // Allocate buffer for pixels
        std::vector<unsigned char> pixels(width * height * 3);

        // Read pixels from framebuffer
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

        // Simple PPM file format (for demonstration)
        std::ofstream file(filename);
        if (!file.is_open()) {
            SE_ERROR("Failed to open file for screenshot: {}", filename);
            return false;
        }

        file << "P3\n" << width << " " << height << "\n255\n";
        
        // Write pixels (flip vertically since OpenGL origin is bottom-left)
        for (int y = height - 1; y >= 0; --y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * 3;
                file << static_cast<int>(pixels[index]) << " "
                     << static_cast<int>(pixels[index + 1]) << " "
                     << static_cast<int>(pixels[index + 2]) << " ";
            }
            file << "\n";
        }

        file.close();
        SE_INFO("Screenshot saved: {}", filename);
        return true;
    }

    void Window::ConfigureWindowHints() {
        // OpenGL version and profile
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_OpenGLProps.majorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_OpenGLProps.minorVersion);
        
        if (m_OpenGLProps.coreProfile) {
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
        
        if (m_OpenGLProps.forwardCompatible) {
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        }

        if (m_OpenGLProps.debugContext) {
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        }

        // Window properties
        glfwWindowHint(GLFW_RESIZABLE, m_WindowProps.resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, m_WindowProps.decorated ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUSED, m_WindowProps.focused ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_MAXIMIZED, m_WindowProps.maximized ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, m_WindowProps.visible ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, m_WindowProps.transparent ? GLFW_TRUE : GLFW_FALSE);

        // Buffer properties
        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
        glfwWindowHint(GLFW_DEPTH_BITS, m_OpenGLProps.depthBits);
        glfwWindowHint(GLFW_STENCIL_BITS, m_OpenGLProps.stencilBits);

        // MSAA
        if (m_WindowProps.samples > 0) {
            glfwWindowHint(GLFW_SAMPLES, m_WindowProps.samples);
        }
    }

    bool Window::InitializeOpenGL() {
        // Load OpenGL functions with GLAD
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            SE_ERROR("Failed to initialize GLAD");
            return false;
        }

        int major = GLAD_VERSION_MAJOR(version);
        int minor = GLAD_VERSION_MINOR(version);
        
        SE_INFO("OpenGL {}.{} loaded successfully", major, minor);
        SE_INFO("OpenGL Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        SE_INFO("OpenGL Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        SE_INFO("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        return true;
    }

    void Window::SetupOpenGLDefaults() {
        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Enable face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        // Set clear color
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set viewport
        glViewport(0, 0, static_cast<GLsizei>(m_WindowProps.width), static_cast<GLsizei>(m_WindowProps.height));

        SE_TRACE("OpenGL default state configured");
    }

    void Window::UpdateWindowProperties() {
        if (!m_Window) {
            return;
        }

        // Update size from actual window
        int width, height;
        glfwGetWindowSize(m_Window, &width, &height);
        m_WindowProps.width = static_cast<uint32_t>(width);
        m_WindowProps.height = static_cast<uint32_t>(height);
    }

} // namespace SilicaEngine