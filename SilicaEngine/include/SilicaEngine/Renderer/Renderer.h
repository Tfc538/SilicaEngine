/**
 * @file Renderer.h
 * @brief High-level OpenGL renderer for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.0.0
 * 
 * Primitive rendering, state management, and statistics tracking.
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

    /// Rendering performance statistics
    struct RendererStats {
        uint32_t drawCalls = 0;
        uint32_t vertices = 0;
        uint32_t triangles = 0;
        uint32_t shaderSwitches = 0;
        uint32_t textureBinds = 0;

        void Reset() {
            drawCalls = 0;
            vertices = 0;
            triangles = 0;
            shaderSwitches = 0;
            textureBinds = 0;
        }
    };

    /// Current OpenGL rendering state
    struct RenderState {
        bool depthTest = true;
        bool blending = false;
        bool cullFace = true;
        bool wireframe = false;
        GLenum cullMode = GL_BACK;
        GLenum blendSrcFactor = GL_SRC_ALPHA;
        GLenum blendDstFactor = GL_ONE_MINUS_SRC_ALPHA;
        glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    };

    /// High-level OpenGL renderer with primitive drawing and state management
    class Renderer {
    public:
        /// Initialize renderer systems
        static bool Initialize();

        /// Cleanup and shutdown
        static void Shutdown();

        /// Begin new frame (clears screen, resets stats)
        static void BeginFrame();

        /// End frame (updates statistics)
        static void EndFrame();

        /// Clear buffers
        static void Clear(bool colorBuffer = true, bool depthBuffer = true, bool stencilBuffer = false);

        /// Set OpenGL viewport
        static void SetViewport(int x, int y, int width, int height);

        /// Set clear color
        static void SetClearColor(const glm::vec4& color);
        static void SetClearColor(float r, float g, float b, float a = 1.0f);

        // Rendering state
        static void SetDepthTest(bool enabled);
        static void SetBlending(bool enabled);
        static void SetCullFace(bool enabled);
        static void SetCullMode(GLenum mode); // GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
        static void SetWireframe(bool enabled);
        static void SetBlendFunc(GLenum srcFactor, GLenum dstFactor);

        // Primitive drawing
        static void DrawTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, 
                               const glm::vec4& color = glm::vec4(1.0f));
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, 
                           const glm::vec4& color = glm::vec4(1.0f));
        static void DrawCube(const glm::vec3& position, const glm::vec3& size, 
                           const glm::vec4& color = glm::vec4(1.0f));
        static void DrawLine(const glm::vec3& start, const glm::vec3& end, 
                           const glm::vec4& color = glm::vec4(1.0f), float width = 1.0f);
        static void DrawGrid(float size, int divisions, const glm::vec4& color = glm::vec4(0.5f));

        // Matrix management
        static void SetViewMatrix(const glm::mat4& view);
        static void SetProjectionMatrix(const glm::mat4& projection);
        static void SetModelMatrix(const glm::mat4& model);
        static const glm::mat4& GetViewMatrix();
        static const glm::mat4& GetProjectionMatrix();
        static const glm::mat4& GetModelMatrix();

        // Statistics and state
        static const RendererStats& GetStats();
        static void ResetStats();
        static const RenderState& GetRenderState();
        static void ApplyRenderState(const RenderState& state);
        static bool CheckGLError(const char* operation = nullptr);
        static const char* GetOpenGLVersion();
        static const char* GetGPUVendor();
        static const char* GetGPURenderer();

    private:
        static bool s_Initialized;
        static RendererStats s_Stats;
        static RenderState s_RenderState;
        static glm::mat4 s_ViewMatrix;
        static glm::mat4 s_ProjectionMatrix;
        static glm::mat4 s_ModelMatrix;
        static std::shared_ptr<Shader> s_DefaultShader;
        
        // Geometry data
        static uint32_t s_QuadVAO, s_QuadVBO;
        static uint32_t s_CubeVAO, s_CubeVBO;
        static uint32_t s_LineVAO, s_LineVBO;

        static void InitializeGeometry();
        static void CleanupGeometry();
        static void UpdateShaderUniforms();
    };

} // namespace SilicaEngine