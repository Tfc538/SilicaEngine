/**
 * @file Renderer.h
 * @brief High-level OpenGL renderer for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Primitive rendering, state management, and statistics tracking.
 */

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SilicaEngine/Core/ErrorCodes.h"
#include "SilicaEngine/Debug/DebugRenderer.h"
#include "SilicaEngine/Renderer/BatchRenderer.h"
#include <memory>
#include <vector>

namespace SilicaEngine {

    // Forward declarations
    class Shader;
    class Texture;
    struct CubeInstance; // Forward declaration for batch rendering

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
        static ErrorResult<void> Initialize();

        /// Cleanup and shutdown
        static ErrorResult<void> Shutdown();

        /// Begin new frame (clears screen, resets stats)
        static void BeginFrame();

        /// End frame (updates statistics)
        static void EndFrame();
        
        /// Render debug information (call after main rendering)
        static void RenderDebugInfo();

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

        // Batch rendering (high-performance instanced rendering)
        static void DrawCubesInstanced(const std::vector<CubeInstance>& instances);
        static void BeginCubeBatch();
        static void EndCubeBatch();
        static void AddCubeToBatch(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
        
        // Textured drawing
        static void DrawTexturedQuad(const glm::vec3& position, const glm::vec2& size,
                                   std::shared_ptr<Texture> texture,
                                   const glm::vec4& tint = glm::vec4(1.0f),
                                   const glm::vec2& uvMin = glm::vec2(0.0f, 0.0f),
                                   const glm::vec2& uvMax = glm::vec2(1.0f, 1.0f));
        
        // Batch textured rendering for efficiency
        static void BeginBatch();
        static void EndBatch();
        static void SubmitQuad(const glm::vec3& position, const glm::vec2& size,
                             std::shared_ptr<Texture> texture,
                             const glm::vec4& tint = glm::vec4(1.0f));
        
        // Texture management
        static void BindTexture(std::shared_ptr<Texture> texture, uint32_t slot = 0);
        static void UnbindTexture(uint32_t slot = 0);

        // Matrix management
        static void SetViewMatrix(const glm::mat4& view);
        static void SetProjectionMatrix(const glm::mat4& projection);
        static void SetModelMatrix(const glm::mat4& model);
        static const glm::mat4& GetViewMatrix();
        static const glm::mat4& GetProjectionMatrix();
                static const glm::mat4& GetModelMatrix();
        static glm::mat4 GetViewProjectionMatrix();
        
        // Statistics and state
        static const RendererStats& GetStats();
        static void ResetStats();
        static void UpdateStats(uint32_t drawCalls, uint32_t vertices, uint32_t triangles);
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
        
        // Textured geometry
        static uint32_t s_TexturedQuadVAO, s_TexturedQuadVBO, s_TexturedQuadEBO;
        
        // Batch rendering
        static bool s_BatchActive;
        static std::vector<float> s_BatchVertices;
        static std::vector<uint32_t> s_BatchIndices;
        static std::shared_ptr<Texture> s_CurrentBatchTexture;

        static void InitializeGeometry();
        static void CleanupGeometry();
        static void UpdateShaderUniforms();
    };

    // OpenGL Error Checking (Debug builds only)
    void CheckGLErrorDebug(const char* operation, const char* file, int line);
    
    // OpenGL Version Validation
    bool ValidateOpenGLVersion(const char* versionString);
    
} // namespace SilicaEngine

// OpenGL Error Checking Macros
#ifdef SILICA_DEBUG
    #define GL_CALL(x) do { \
        x; \
        SilicaEngine::CheckGLErrorDebug(#x, __FILE__, __LINE__); \
    } while(0)
#else
    #define GL_CALL(x) x
#endif