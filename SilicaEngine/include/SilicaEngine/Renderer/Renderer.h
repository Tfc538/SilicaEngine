/**
 * @file Renderer.h
 * @brief Main renderer class for SilicaEngine
 * @version 1.0.0
 * 
 * Provides high-level rendering functionality with modern OpenGL features.
 * Supports basic geometric primitives, shaders, and state management.
 */

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

namespace SilicaEngine {

    // Forward declarations
    class Shader;

    /**
     * @struct RendererStats
     * @brief Statistics collected during rendering
     */
    struct RendererStats {
        uint32_t drawCalls = 0;         ///< Number of draw calls
        uint32_t vertices = 0;          ///< Number of vertices rendered
        uint32_t triangles = 0;         ///< Number of triangles rendered
        uint32_t shaderSwitches = 0;    ///< Number of shader switches
        uint32_t textureBinds = 0;      ///< Number of texture binds

        /**
         * @brief Reset all statistics to zero
         */
        void Reset() {
            drawCalls = 0;
            vertices = 0;
            triangles = 0;
            shaderSwitches = 0;
            textureBinds = 0;
        }
    };

    /**
     * @struct RenderState
     * @brief Current rendering state
     */
    struct RenderState {
        bool depthTest = true;              ///< Depth testing enabled
        bool blending = false;              ///< Alpha blending enabled
        bool cullFace = true;               ///< Face culling enabled
        bool wireframe = false;             ///< Wireframe mode
        GLenum cullMode = GL_BACK;          ///< Culling mode
        GLenum blendSrcFactor = GL_SRC_ALPHA;   ///< Blend source factor
        GLenum blendDstFactor = GL_ONE_MINUS_SRC_ALPHA; ///< Blend destination factor
        glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f); ///< Clear color
    };

    /**
     * @class Renderer
     * @brief High-level OpenGL renderer
     * 
     * Provides a clean interface for common rendering operations,
     * state management, and primitive rendering.
     */
    class Renderer {
    public:
        /**
         * @brief Initialize the renderer
         * @return true if initialization was successful
         */
        static bool Initialize();

        /**
         * @brief Shutdown the renderer and cleanup resources
         */
        static void Shutdown();

        /**
         * @brief Begin a new frame
         * 
         * Clears the screen and prepares for rendering.
         */
        static void BeginFrame();

        /**
         * @brief End the current frame
         * 
         * Finishes rendering and updates statistics.
         */
        static void EndFrame();

        /**
         * @brief Clear the screen with the current clear color
         * @param colorBuffer Clear color buffer
         * @param depthBuffer Clear depth buffer
         * @param stencilBuffer Clear stencil buffer
         */
        static void Clear(bool colorBuffer = true, bool depthBuffer = true, bool stencilBuffer = false);

        /**
         * @brief Set the viewport
         * @param x X coordinate
         * @param y Y coordinate
         * @param width Viewport width
         * @param height Viewport height
         */
        static void SetViewport(int x, int y, int width, int height);

        /**
         * @brief Set the clear color
         * @param color Clear color as RGBA
         */
        static void SetClearColor(const glm::vec4& color);

        /**
         * @brief Set the clear color
         * @param r Red component (0-1)
         * @param g Green component (0-1)
         * @param b Blue component (0-1)
         * @param a Alpha component (0-1)
         */
        static void SetClearColor(float r, float g, float b, float a = 1.0f);

        /**
         * @brief Enable or disable depth testing
         * @param enabled true to enable depth testing
         */
        static void SetDepthTest(bool enabled);

        /**
         * @brief Enable or disable alpha blending
         * @param enabled true to enable blending
         */
        static void SetBlending(bool enabled);

        /**
         * @brief Enable or disable face culling
         * @param enabled true to enable face culling
         */
        static void SetCullFace(bool enabled);

        /**
         * @brief Set culling mode
         * @param mode GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK
         */
        static void SetCullMode(GLenum mode);

        /**
         * @brief Enable or disable wireframe mode
         * @param enabled true for wireframe mode
         */
        static void SetWireframe(bool enabled);

        /**
         * @brief Set blend function
         * @param srcFactor Source blend factor
         * @param dstFactor Destination blend factor
         */
        static void SetBlendFunc(GLenum srcFactor, GLenum dstFactor);

        /**
         * @brief Draw a triangle
         * @param v1 First vertex
         * @param v2 Second vertex
         * @param v3 Third vertex
         * @param color Triangle color
         */
        static void DrawTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, 
                               const glm::vec4& color = glm::vec4(1.0f));

        /**
         * @brief Draw a quad
         * @param position Position of the quad center
         * @param size Size of the quad
         * @param color Quad color
         */
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, 
                           const glm::vec4& color = glm::vec4(1.0f));

        /**
         * @brief Draw a cube
         * @param position Position of the cube center
         * @param size Size of the cube
         * @param color Cube color
         */
        static void DrawCube(const glm::vec3& position, const glm::vec3& size, 
                           const glm::vec4& color = glm::vec4(1.0f));

        /**
         * @brief Draw a line
         * @param start Start position
         * @param end End position
         * @param color Line color
         * @param width Line width
         */
        static void DrawLine(const glm::vec3& start, const glm::vec3& end, 
                           const glm::vec4& color = glm::vec4(1.0f), float width = 1.0f);

        /**
         * @brief Draw a grid
         * @param size Grid size
         * @param divisions Number of divisions
         * @param color Grid color
         */
        static void DrawGrid(float size, int divisions, const glm::vec4& color = glm::vec4(0.5f));

        /**
         * @brief Set the view matrix
         * @param view View matrix
         */
        static void SetViewMatrix(const glm::mat4& view);

        /**
         * @brief Set the projection matrix
         * @param projection Projection matrix
         */
        static void SetProjectionMatrix(const glm::mat4& projection);

        /**
         * @brief Set the model matrix
         * @param model Model matrix
         */
        static void SetModelMatrix(const glm::mat4& model);

        /**
         * @brief Get the current view matrix
         * @return Current view matrix
         */
        static const glm::mat4& GetViewMatrix();

        /**
         * @brief Get the current projection matrix
         * @return Current projection matrix
         */
        static const glm::mat4& GetProjectionMatrix();

        /**
         * @brief Get the current model matrix
         * @return Current model matrix
         */
        static const glm::mat4& GetModelMatrix();

        /**
         * @brief Get rendering statistics
         * @return Current frame statistics
         */
        static const RendererStats& GetStats();

        /**
         * @brief Reset rendering statistics
         */
        static void ResetStats();

        /**
         * @brief Get current render state
         * @return Current render state
         */
        static const RenderState& GetRenderState();

        /**
         * @brief Apply a render state
         * @param state Render state to apply
         */
        static void ApplyRenderState(const RenderState& state);

        /**
         * @brief Check for OpenGL errors and log them
         * @param operation Optional operation description
         * @return true if no errors were found
         */
        static bool CheckGLError(const char* operation = nullptr);

        /**
         * @brief Get OpenGL version string
         * @return OpenGL version string
         */
        static const char* GetOpenGLVersion();

        /**
         * @brief Get GPU vendor string
         * @return GPU vendor string
         */
        static const char* GetGPUVendor();

        /**
         * @brief Get GPU renderer string
         * @return GPU renderer string
         */
        static const char* GetGPURenderer();

    private:
        static bool s_Initialized;                     ///< Initialization state
        static RendererStats s_Stats;                  ///< Rendering statistics
        static RenderState s_RenderState;              ///< Current render state
        static glm::mat4 s_ViewMatrix;                 ///< View matrix
        static glm::mat4 s_ProjectionMatrix;           ///< Projection matrix
        static glm::mat4 s_ModelMatrix;                ///< Model matrix
        static std::shared_ptr<Shader> s_DefaultShader; ///< Default shader
        
        // Geometry data
        static uint32_t s_QuadVAO, s_QuadVBO;          ///< Quad vertex data
        static uint32_t s_CubeVAO, s_CubeVBO;          ///< Cube vertex data
        static uint32_t s_LineVAO, s_LineVBO;          ///< Line vertex data

        /**
         * @brief Initialize basic geometry
         */
        static void InitializeGeometry();

        /**
         * @brief Cleanup geometry resources
         */
        static void CleanupGeometry();

        /**
         * @brief Update shader uniforms
         */
        static void UpdateShaderUniforms();
    };

} // namespace SilicaEngine