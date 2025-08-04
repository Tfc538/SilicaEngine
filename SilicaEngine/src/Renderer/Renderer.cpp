/**
 * @file Renderer.cpp
 * @brief OpenGL renderer implementation
 * @author Tim Gatzke <post@tim-gatzke.de>
 */

#include "SilicaEngine/Renderer/Renderer.h"
#include "SilicaEngine/Renderer/Shader.h"
#include "SilicaEngine/Renderer/Texture.h"
#include "SilicaEngine/SilicaEngine.h"  // For logging macros
#include "SilicaEngine/Debug/Profiler.h"
#include <glm/gtc/matrix_transform.hpp>
#include <mutex>

namespace SilicaEngine {

    // Static member definitions - using initialization-on-first-use pattern for safety
    bool& GetInitializedRef() {
        static bool s_Initialized = false;
        return s_Initialized;
    }
    
    RendererStats& GetStatsRef() {
        static RendererStats s_Stats{};
        return s_Stats;
    }
    
    RenderState& GetRenderStateRef() {
        static RenderState s_RenderState{};
        return s_RenderState;
    }
    
    glm::mat4& GetViewMatrixRef() {
        static glm::mat4 s_ViewMatrix = glm::mat4(1.0f);
        return s_ViewMatrix;
    }
    
    glm::mat4& GetProjectionMatrixRef() {
        static glm::mat4 s_ProjectionMatrix = glm::mat4(1.0f);
        return s_ProjectionMatrix;
    }
    
    glm::mat4& GetModelMatrixRef() {
        static glm::mat4 s_ModelMatrix = glm::mat4(1.0f);
        return s_ModelMatrix;
    }
    
    std::shared_ptr<Shader>& GetDefaultShaderRef() {
        static std::shared_ptr<Shader> s_DefaultShader;
        return s_DefaultShader;
    }
    
    // Accessor macros for cleaner code
    #define s_Initialized GetInitializedRef()
    #define s_Stats GetStatsRef()
    #define s_RenderState GetRenderStateRef()
    #define s_ViewMatrix GetViewMatrixRef()
    #define s_ProjectionMatrix GetProjectionMatrixRef()
    #define s_ModelMatrix GetModelMatrixRef()
    #define s_DefaultShader GetDefaultShaderRef()
    
    // Geometry VAOs and VBOs - using initialization-on-first-use pattern
    uint32_t& GetQuadVAO() { static uint32_t s_QuadVAO = 0; return s_QuadVAO; }
    uint32_t& GetQuadVBO() { static uint32_t s_QuadVBO = 0; return s_QuadVBO; }
    uint32_t& GetCubeVAO() { static uint32_t s_CubeVAO = 0; return s_CubeVAO; }
    uint32_t& GetCubeVBO() { static uint32_t s_CubeVBO = 0; return s_CubeVBO; }
    uint32_t& GetLineVAO() { static uint32_t s_LineVAO = 0; return s_LineVAO; }
    uint32_t& GetLineVBO() { static uint32_t s_LineVBO = 0; return s_LineVBO; }
    
    // Textured geometry
    uint32_t& GetTexturedQuadVAO() { static uint32_t s_TexturedQuadVAO = 0; return s_TexturedQuadVAO; }
    uint32_t& GetTexturedQuadVBO() { static uint32_t s_TexturedQuadVBO = 0; return s_TexturedQuadVBO; }
    uint32_t& GetTexturedQuadEBO() { static uint32_t s_TexturedQuadEBO = 0; return s_TexturedQuadEBO; }
    
    // Batch rendering
    bool& GetBatchActive() { static bool s_BatchActive = false; return s_BatchActive; }
    std::vector<float>& GetBatchVertices() { static std::vector<float> s_BatchVertices; return s_BatchVertices; }
    std::vector<uint32_t>& GetBatchIndices() { static std::vector<uint32_t> s_BatchIndices; return s_BatchIndices; }
    std::shared_ptr<Texture>& GetCurrentBatchTexture() { static std::shared_ptr<Texture> s_CurrentBatchTexture; return s_CurrentBatchTexture; }
    
    // Additional accessor macros
    #define s_QuadVAO GetQuadVAO()
    #define s_QuadVBO GetQuadVBO()
    #define s_CubeVAO GetCubeVAO()
    #define s_CubeVBO GetCubeVBO()
    #define s_LineVAO GetLineVAO()
    #define s_LineVBO GetLineVBO()
    #define s_TexturedQuadVAO GetTexturedQuadVAO()
    #define s_TexturedQuadVBO GetTexturedQuadVBO()
    #define s_TexturedQuadEBO GetTexturedQuadEBO()
    #define s_BatchActive GetBatchActive()
    #define s_BatchVertices GetBatchVertices()
    #define s_BatchIndices GetBatchIndices()
    #define s_CurrentBatchTexture GetCurrentBatchTexture()

    // Batch renderer instance
    std::unique_ptr<BatchRenderer>& GetBatchRenderer() { 
        static std::unique_ptr<BatchRenderer> s_BatchRenderer; 
        return s_BatchRenderer; 
    }
    #define s_BatchRenderer GetBatchRenderer()

    std::mutex& GetBatchMutex() {
        static std::mutex s_BatchMutex;
        return s_BatchMutex;
    }
    #define s_BatchMutex GetBatchMutex()

    ErrorResult<void> Renderer::Initialize() {
        SE_PROFILE_FUNCTION();
        
        if (s_Initialized) {
            return ErrorResult<void>::Success();
        }

        SE_INFO("Initializing renderer...");

        // Check OpenGL version
        const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

        SE_INFO("OpenGL Info:");
        SE_INFO("  Version: {}", version ? version : "Unknown");
        SE_INFO("  Vendor: {}", vendor ? vendor : "Unknown");
        SE_INFO("  Renderer: {}", renderer ? renderer : "Unknown");
        
        // Validate OpenGL version requirements
        if (!ValidateOpenGLVersion(version)) {
            SE_ERROR("OpenGL version validation failed");
            return ErrorResult<void>::Error(EngineError::OpenGLVersionUnsupported, "OpenGL version validation failed");
        }

        // Create default shader
        s_DefaultShader = Shader::CreateDefault();
        if (!s_DefaultShader) {
            SE_ERROR("Failed to create default shader");
            return ErrorResult<void>::Error(EngineError::ShaderCompilationFailed, "Failed to create default shader");
        }

        // Initialize geometry
        InitializeGeometry();

        // Initialize batch renderer
        s_BatchRenderer = std::make_unique<BatchRenderer>();
        auto batchResult = s_BatchRenderer->Initialize();
        if (!batchResult) {
            SE_ERROR("Failed to initialize batch renderer: {}", batchResult.errorMessage);
            return ErrorResult<void>::Error(EngineError::InitializationFailed, 
                                          "Failed to initialize batch renderer: " + batchResult.errorMessage);
        }

        // Set default render state
        s_RenderState = RenderState{};
        ApplyRenderState(s_RenderState);

        // Set default matrices
        s_ViewMatrix = glm::mat4(1.0f);
        s_ProjectionMatrix = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
        s_ModelMatrix = glm::mat4(1.0f);

        s_Initialized = true;
        SE_INFO("Renderer initialized successfully");
        return ErrorResult<void>::Success();
    }

    ErrorResult<void> Renderer::Shutdown() {
        SE_PROFILE_FUNCTION();
        
        if (!s_Initialized) {
            return ErrorResult<void>::Success();
        }

        SE_INFO("Shutting down renderer...");

        // Cleanup geometry
        CleanupGeometry();

        // Shutdown batch renderer
        if (s_BatchRenderer) {
            s_BatchRenderer->Shutdown();
            s_BatchRenderer.reset();
        }

        // Release default shader
        s_DefaultShader.reset();

        s_Initialized = false;
        SE_INFO("Renderer shutdown complete");
        return ErrorResult<void>::Success();
    }

    void Renderer::BeginFrame() {
        if (!s_Initialized) {
            return;
        }

        // Reset stats for new frame
        s_Stats.Reset();
    }

    void Renderer::EndFrame() {
        if (!s_Initialized) {
            return;
        }

        // Frame complete - stats are now available
    }
    
    void Renderer::RenderDebugInfo() {
        SE_PROFILE_FUNCTION();
        
        if (!s_Initialized) {
            return;
        }
        
        // Render debug information using DebugRenderer
        DebugRenderer::Render(s_ViewMatrix, s_ProjectionMatrix);
    }

    void Renderer::Clear(bool colorBuffer, bool depthBuffer, bool stencilBuffer) {
        GLbitfield mask = 0;
        
        if (colorBuffer) mask |= GL_COLOR_BUFFER_BIT;
        if (depthBuffer) mask |= GL_DEPTH_BUFFER_BIT;
        if (stencilBuffer) mask |= GL_STENCIL_BUFFER_BIT;
        
        glClear(mask);
    }

    void Renderer::SetViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

    void Renderer::SetClearColor(const glm::vec4& color) {
        glClearColor(color.r, color.g, color.b, color.a);
        s_RenderState.clearColor = color;
    }

    void Renderer::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
        s_RenderState.clearColor = glm::vec4(r, g, b, a);
    }

    void Renderer::SetDepthTest(bool enabled) {
        if (enabled) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        s_RenderState.depthTest = enabled;
    }

    void Renderer::SetBlending(bool enabled) {
        if (enabled) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
        s_RenderState.blending = enabled;
    }

    void Renderer::SetCullFace(bool enabled) {
        if (enabled) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
        s_RenderState.cullFace = enabled;
    }

    void Renderer::SetCullMode(GLenum mode) {
        glCullFace(mode);
        s_RenderState.cullMode = mode;
    }

    void Renderer::SetWireframe(bool enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
        s_RenderState.wireframe = enabled;
    }

    void Renderer::SetBlendFunc(GLenum srcFactor, GLenum dstFactor) {
        glBlendFunc(srcFactor, dstFactor);
        s_RenderState.blendSrcFactor = srcFactor;
        s_RenderState.blendDstFactor = dstFactor;
    }

    void Renderer::DrawTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec4& color) {
        if (!s_DefaultShader) return;

        // Vertex data: position + color
        float vertices[] = {
            v1.x, v1.y, v1.z, color.r, color.g, color.b, color.a,
            v2.x, v2.y, v2.z, color.r, color.g, color.b, color.a,
            v3.x, v3.y, v3.z, color.r, color.g, color.b, color.a
        };

        // Create temporary VAO and VBO
        uint32_t VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Bind shader and set uniforms
        s_DefaultShader->Bind();
        UpdateShaderUniforms();

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Cleanup
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

        // Update stats
        s_Stats.drawCalls++;
        s_Stats.vertices += 3;
        s_Stats.triangles += 1;
    }

    void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
        if (!s_DefaultShader || s_QuadVAO == 0) return;

        // Create transformation matrix
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
        transform = glm::scale(transform, glm::vec3(size.x, size.y, 1.0f));

        // Bind shader and set uniforms
        s_DefaultShader->Bind();
        s_DefaultShader->SetMat4("u_ViewProjection", s_ProjectionMatrix * s_ViewMatrix);
        s_DefaultShader->SetMat4("u_Transform", transform);

        // Update quad color
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, color.r, color.g, color.b, color.a,
             0.5f, -0.5f, 0.0f, color.r, color.g, color.b, color.a,
             0.5f,  0.5f, 0.0f, color.r, color.g, color.b, color.a,
            -0.5f,  0.5f, 0.0f, color.r, color.g, color.b, color.a
        };

        glBindVertexArray(s_QuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_QuadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Draw quad as two triangles
        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
        
        uint32_t EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glDeleteBuffers(1, &EBO);

        // Update stats
        s_Stats.drawCalls++;
        s_Stats.vertices += 4;
        s_Stats.triangles += 2;
    }

    void Renderer::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color) {
        if (!s_DefaultShader || s_CubeVAO == 0) return;

        // Create transformation matrix
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
        transform = glm::scale(transform, size);

        // Bind shader and set uniforms
        s_DefaultShader->Bind();
        s_DefaultShader->SetMat4("u_ViewProjection", s_ProjectionMatrix * s_ViewMatrix);
        s_DefaultShader->SetMat4("u_Transform", transform);
        s_DefaultShader->SetVec4("u_Color", color);

        glBindVertexArray(s_CubeVAO);

        // Draw cube (36 vertices for 12 triangles)
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Update stats
        s_Stats.drawCalls++;
        s_Stats.vertices += 36;
        s_Stats.triangles += 12;
    }

    void Renderer::DrawCubesInstanced(const std::vector<CubeInstance>& instances) {
        if (!s_Initialized || instances.empty()) {
            return;
        }

        // Use batch renderer for instanced rendering
        if (s_BatchRenderer) {
            for (const auto& instance : instances) {
                glm::vec3 position = glm::vec3(instance.transform[3]);
                glm::vec3 scale = glm::vec3(
                    glm::length(glm::vec3(instance.transform[0])),
                    glm::length(glm::vec3(instance.transform[1])),
                    glm::length(glm::vec3(instance.transform[2]))
                );
                s_BatchRenderer->AddCube(position, scale, instance.color);
            }
            s_BatchRenderer->FlushBatch();
        }
    }

    void Renderer::BeginCubeBatch() {
        if (!s_Initialized) {
            return;
        }
        // Batch is automatically managed by BatchRenderer
    }

    void Renderer::EndCubeBatch() {
        if (!s_Initialized) {
            return;
        }
        
        // Flush any remaining cubes in the batch
        if (s_BatchRenderer) {
            s_BatchRenderer->FlushBatch();
        }
    }

    void Renderer::AddCubeToBatch(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color) {
        if (!s_Initialized) {
            return;
        }
        
        if (s_BatchRenderer) {
            s_BatchRenderer->AddCube(position, size, color);
        }
    }

    void Renderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width) {
        if (!s_DefaultShader) return;

        // Set line width
        glLineWidth(width);

        // Line vertices
        float vertices[] = {
            start.x, start.y, start.z, color.r, color.g, color.b, color.a,
            end.x,   end.y,   end.z,   color.r, color.g, color.b, color.a
        };

        glBindVertexArray(s_LineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_LineVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Bind shader and set uniforms
        s_DefaultShader->Bind();
        UpdateShaderUniforms();

        // Draw line
        glDrawArrays(GL_LINES, 0, 2);

        // Reset line width
        glLineWidth(1.0f);

        // Update stats
        s_Stats.drawCalls++;
        s_Stats.vertices += 2;
    }

    void Renderer::DrawGrid(float size, int divisions, const glm::vec4& color) {
        if (!s_DefaultShader) return;

        float step = size / static_cast<float>(divisions);
        float halfSize = size * 0.5f;

        // Draw horizontal lines
        for (int i = 0; i <= divisions; ++i) {
            float y = -halfSize + static_cast<float>(i) * step;
            DrawLine(glm::vec3(-halfSize, y, 0.0f), glm::vec3(halfSize, y, 0.0f), color);
        }

        // Draw vertical lines
        for (int i = 0; i <= divisions; ++i) {
            float x = -halfSize + static_cast<float>(i) * step;
            DrawLine(glm::vec3(x, -halfSize, 0.0f), glm::vec3(x, halfSize, 0.0f), color);
        }
    }

    void Renderer::DrawTexturedQuad(const glm::vec3& position, const glm::vec2& size,
                                   std::shared_ptr<Texture> texture,
                                   const glm::vec4& tint,
                                   const glm::vec2& uvMin,
                                   const glm::vec2& uvMax) {
        if (!s_DefaultShader || s_TexturedQuadVAO == 0 || !texture) return;

        // Create transformation matrix
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
        transform = glm::scale(transform, glm::vec3(size.x, size.y, 1.0f));

        // Bind texture
        texture->Bind(0);

        // Bind shader and set uniforms
        s_DefaultShader->Bind();
        s_DefaultShader->SetMat4("u_ViewProjection", s_ProjectionMatrix * s_ViewMatrix);
        s_DefaultShader->SetMat4("u_Model", transform);
        s_DefaultShader->SetVec4("u_Color", tint);
        s_DefaultShader->SetInt("u_Texture", 0);

        // Update textured quad vertices with UV coordinates
        float vertices[] = {
            // positions        // texture coords
            -0.5f, -0.5f, 0.0f, uvMin.x, uvMin.y,
             0.5f, -0.5f, 0.0f, uvMax.x, uvMin.y,
             0.5f,  0.5f, 0.0f, uvMax.x, uvMax.y,
            -0.5f,  0.5f, 0.0f, uvMin.x, uvMax.y
        };

        glBindVertexArray(s_TexturedQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_TexturedQuadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Draw quad using element buffer
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Update stats
        s_Stats.drawCalls++;
        s_Stats.vertices += 4;
        s_Stats.triangles += 2;
        s_Stats.textureBinds++;
    }

    void Renderer::BeginBatch() {
        std::lock_guard<std::mutex> lock(s_BatchMutex);
        if (s_BatchActive) return;

        s_BatchActive = true;
        s_BatchVertices.clear();
        s_BatchIndices.clear();
        s_CurrentBatchTexture.reset();
    }

    void Renderer::EndBatch() {
        std::lock_guard<std::mutex> lock(s_BatchMutex);
        if (!s_BatchActive || s_BatchVertices.empty()) {
            s_BatchActive = false;
            return;
        }

        if (!s_DefaultShader || s_TexturedQuadVAO == 0 || !s_CurrentBatchTexture) {
            s_BatchActive = false;
            return;
        }

        // Bind texture and shader
        s_CurrentBatchTexture->Bind(0);
        s_DefaultShader->Bind();
        s_DefaultShader->SetMat4("u_ViewProjection", s_ProjectionMatrix * s_ViewMatrix);
        s_DefaultShader->SetMat4("u_Model", glm::mat4(1.0f));
        s_DefaultShader->SetVec4("u_Color", glm::vec4(1.0f));
        s_DefaultShader->SetInt("u_Texture", 0);

        // Upload batch data
        glBindVertexArray(s_TexturedQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_TexturedQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, s_BatchVertices.size() * sizeof(float), 
                     s_BatchVertices.data(), GL_DYNAMIC_DRAW);

        // Update vertex attributes for batch format (pos + UV + color)
        // Position attribute (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute (location = 1) 
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Color attribute (location = 2)
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_TexturedQuadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_BatchIndices.size() * sizeof(uint32_t),
                     s_BatchIndices.data(), GL_DYNAMIC_DRAW);

        // Draw batch
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(s_BatchIndices.size()), GL_UNSIGNED_INT, 0);

        // Update stats
        s_Stats.drawCalls++;
        s_Stats.vertices += static_cast<uint32_t>(s_BatchVertices.size() / 8); // 8 floats per vertex (pos + UV + color)
        s_Stats.triangles += static_cast<uint32_t>(s_BatchIndices.size() / 3);
        s_Stats.textureBinds++;

        s_BatchActive = false;
    }

    void Renderer::SubmitQuad(const glm::vec3& position, const glm::vec2& size,
                             std::shared_ptr<Texture> texture,
                             const glm::vec4& tint) {
        std::lock_guard<std::mutex> lock(s_BatchMutex);
        if (!s_BatchActive || !texture) return;

        // Check if adding this quad would exceed batch limits
        const size_t MAX_VERTICES = 10000; // 4 vertices per quad = 2500 quads max
        const size_t MAX_INDICES = 15000;  // 6 indices per quad = 2500 quads max
        
        if (s_BatchVertices.size() / 8 >= MAX_VERTICES || s_BatchIndices.size() >= MAX_INDICES) {
            // Flush current batch and start new one
            EndBatch();
            BeginBatch();
        }

        // If this is the first quad or different texture, set as current
        if (!s_CurrentBatchTexture || s_CurrentBatchTexture != texture) {
            // If we already have a different texture, end current batch and start new one
            if (s_CurrentBatchTexture && s_CurrentBatchTexture != texture) {
                EndBatch();
                BeginBatch();
            }
            s_CurrentBatchTexture = texture;
        }

        // Calculate vertex positions
        glm::vec3 bottomLeft = position - glm::vec3(size.x * 0.5f, size.y * 0.5f, 0.0f);
        glm::vec3 bottomRight = position + glm::vec3(size.x * 0.5f, -size.y * 0.5f, 0.0f);
        glm::vec3 topRight = position + glm::vec3(size.x * 0.5f, size.y * 0.5f, 0.0f);
        glm::vec3 topLeft = position + glm::vec3(-size.x * 0.5f, size.y * 0.5f, 0.0f);

        uint32_t baseIndex = static_cast<uint32_t>(s_BatchVertices.size() / 9); // 9 floats per vertex (pos + UV + RGBA color)

        // Add vertices (position + UV + RGBA color)
        // Bottom left
        s_BatchVertices.insert(s_BatchVertices.end(), {bottomLeft.x, bottomLeft.y, bottomLeft.z, 0.0f, 0.0f, tint.r, tint.g, tint.b, tint.a});
        // Bottom right
        s_BatchVertices.insert(s_BatchVertices.end(), {bottomRight.x, bottomRight.y, bottomRight.z, 1.0f, 0.0f, tint.r, tint.g, tint.b, tint.a});
        // Top right
        s_BatchVertices.insert(s_BatchVertices.end(), {topRight.x, topRight.y, topRight.z, 1.0f, 1.0f, tint.r, tint.g, tint.b, tint.a});
        // Top left
        s_BatchVertices.insert(s_BatchVertices.end(), {topLeft.x, topLeft.y, topLeft.z, 0.0f, 1.0f, tint.r, tint.g, tint.b, tint.a});

        // Add indices (two triangles)
        s_BatchIndices.insert(s_BatchIndices.end(), {
            baseIndex, baseIndex + 1, baseIndex + 2,  // First triangle
            baseIndex + 2, baseIndex + 3, baseIndex   // Second triangle
        });
    }

    void Renderer::BindTexture(std::shared_ptr<Texture> texture, uint32_t slot) {
        if (texture) {
            texture->Bind(static_cast<int>(slot));
            s_Stats.textureBinds++;
        }
    }

    void Renderer::UnbindTexture(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Renderer::SetViewMatrix(const glm::mat4& view) {
        s_ViewMatrix = view;
    }

    void Renderer::SetProjectionMatrix(const glm::mat4& projection) {
        s_ProjectionMatrix = projection;
    }

    void Renderer::SetModelMatrix(const glm::mat4& model) {
        s_ModelMatrix = model;
    }

    const glm::mat4& Renderer::GetViewMatrix() {
        return s_ViewMatrix;
    }

    const glm::mat4& Renderer::GetProjectionMatrix() {
        return s_ProjectionMatrix;
    }

    const glm::mat4& Renderer::GetModelMatrix() {
        return s_ModelMatrix;
    }

    glm::mat4 Renderer::GetViewProjectionMatrix() {
        return s_ProjectionMatrix * s_ViewMatrix;
    }

    const RendererStats& Renderer::GetStats() {
        return s_Stats;
    }

    void Renderer::ResetStats() {
        s_Stats.Reset();
    }

    void Renderer::UpdateStats(uint32_t drawCalls, uint32_t vertices, uint32_t triangles) {
        s_Stats.drawCalls += drawCalls;
        s_Stats.vertices += vertices;
        s_Stats.triangles += triangles;
    }

    const RenderState& Renderer::GetRenderState() {
        return s_RenderState;
    }

    void Renderer::ApplyRenderState(const RenderState& state) {
        SetDepthTest(state.depthTest);
        SetBlending(state.blending);
        SetCullFace(state.cullFace);
        SetCullMode(state.cullMode);
        SetWireframe(state.wireframe);
        SetBlendFunc(state.blendSrcFactor, state.blendDstFactor);
        SetClearColor(state.clearColor);
        
        s_RenderState = state;
    }

    bool Renderer::CheckGLError(const char* operation) {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            const char* errorString = "Unknown error";
            
            switch (error) {
                case GL_INVALID_ENUM: errorString = "GL_INVALID_ENUM"; break;
                case GL_INVALID_VALUE: errorString = "GL_INVALID_VALUE"; break;
                case GL_INVALID_OPERATION: errorString = "GL_INVALID_OPERATION"; break;
                case GL_OUT_OF_MEMORY: errorString = "GL_OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: errorString = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            
            if (operation) {
                SE_ERROR("OpenGL error in {}: {} ({})", operation, errorString, error);
            } else {
                SE_ERROR("OpenGL error: {} ({})", errorString, error);
            }
            
            return false;
        }
        
        return true;
    }

    const char* Renderer::GetOpenGLVersion() {
        return reinterpret_cast<const char*>(glGetString(GL_VERSION));
    }

    const char* Renderer::GetGPUVendor() {
        return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    }

    const char* Renderer::GetGPURenderer() {
        return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    }

    void Renderer::InitializeGeometry() {
        // Initialize quad
        float quadVertices[] = {
            // positions      // colors
            -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
        };

        glGenVertexArrays(1, &s_QuadVAO);
        glGenBuffers(1, &s_QuadVBO);

        glBindVertexArray(s_QuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_QuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Initialize cube with correct CCW winding order for all faces
        float cubeVertices[] = {
            // ---------- Front (+Z) - CCW when viewed from outside
            -0.5f,-0.5f, 0.5f, 1,1,1,1,  // bottom-left
             0.5f,-0.5f, 0.5f, 1,1,1,1,  // bottom-right
             0.5f, 0.5f, 0.5f, 1,1,1,1,  // top-right
            -0.5f,-0.5f, 0.5f, 1,1,1,1,  // bottom-left
             0.5f, 0.5f, 0.5f, 1,1,1,1,  // top-right
            -0.5f, 0.5f, 0.5f, 1,1,1,1,  // top-left
        
            // ---------- Back (−Z) - CCW when viewed from outside
             0.5f,-0.5f,-0.5f, 1,1,1,1,  // bottom-left (from back view)
            -0.5f,-0.5f,-0.5f, 1,1,1,1,  // bottom-right (from back view)
            -0.5f, 0.5f,-0.5f, 1,1,1,1,  // top-right (from back view)
             0.5f,-0.5f,-0.5f, 1,1,1,1,  // bottom-left (from back view)
            -0.5f, 0.5f,-0.5f, 1,1,1,1,  // top-right (from back view)
             0.5f, 0.5f,-0.5f, 1,1,1,1,  // top-left (from back view)
        
            // ---------- Left (−X) - CCW when viewed from outside
            -0.5f,-0.5f,-0.5f, 1,1,1,1,  // bottom-left
            -0.5f,-0.5f, 0.5f, 1,1,1,1,  // bottom-right
            -0.5f, 0.5f, 0.5f, 1,1,1,1,  // top-right
            -0.5f,-0.5f,-0.5f, 1,1,1,1,  // bottom-left
            -0.5f, 0.5f, 0.5f, 1,1,1,1,  // top-right
            -0.5f, 0.5f,-0.5f, 1,1,1,1,  // top-left
        
            // ---------- Right (+X) - CCW when viewed from outside
             0.5f,-0.5f, 0.5f, 1,1,1,1,  // bottom-left
             0.5f,-0.5f,-0.5f, 1,1,1,1,  // bottom-right
             0.5f, 0.5f,-0.5f, 1,1,1,1,  // top-right
             0.5f,-0.5f, 0.5f, 1,1,1,1,  // bottom-left
             0.5f, 0.5f,-0.5f, 1,1,1,1,  // top-right
             0.5f, 0.5f, 0.5f, 1,1,1,1,  // top-left
        
            // ---------- Top (+Y) - CCW when viewed from outside
            -0.5f, 0.5f, 0.5f, 1,1,1,1,  // bottom-left (from top view)
             0.5f, 0.5f, 0.5f, 1,1,1,1,  // bottom-right (from top view)
             0.5f, 0.5f,-0.5f, 1,1,1,1,  // top-right (from top view)
            -0.5f, 0.5f, 0.5f, 1,1,1,1,  // bottom-left (from top view)
             0.5f, 0.5f,-0.5f, 1,1,1,1,  // top-right (from top view)
            -0.5f, 0.5f,-0.5f, 1,1,1,1,  // top-left (from top view)
        
            // ---------- Bottom (−Y) - CCW when viewed from outside
            -0.5f,-0.5f,-0.5f, 1,1,1,1,  // bottom-left (from bottom view)
             0.5f,-0.5f,-0.5f, 1,1,1,1,  // bottom-right (from bottom view)
             0.5f,-0.5f, 0.5f, 1,1,1,1,  // top-right (from bottom view)
            -0.5f,-0.5f,-0.5f, 1,1,1,1,  // bottom-left (from bottom view)
             0.5f,-0.5f, 0.5f, 1,1,1,1,  // top-right (from bottom view)
            -0.5f,-0.5f, 0.5f, 1,1,1,1,  // top-left (from bottom view)
        };

        glGenVertexArrays(1, &s_CubeVAO);
        glGenBuffers(1, &s_CubeVBO);

        glBindVertexArray(s_CubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_CubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Initialize line VAO (will be updated per draw call)
        glGenVertexArrays(1, &s_LineVAO);
        glGenBuffers(1, &s_LineVBO);

        glBindVertexArray(s_LineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_LineVBO);
        glBufferData(GL_ARRAY_BUFFER, 2 * 7 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Initialize textured quad (position + UV coordinates)
        float texturedQuadVertices[] = {
            // positions        // texture coords
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };

        uint32_t texturedQuadIndices[] = {
            0, 1, 2, 2, 3, 0
        };

        glGenVertexArrays(1, &s_TexturedQuadVAO);
        glGenBuffers(1, &s_TexturedQuadVBO);
        glGenBuffers(1, &s_TexturedQuadEBO);

        glBindVertexArray(s_TexturedQuadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, s_TexturedQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texturedQuadVertices), texturedQuadVertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_TexturedQuadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texturedQuadIndices), texturedQuadIndices, GL_STATIC_DRAW);

        // Position attribute (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute (location = 1) 
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Unbind
        glBindVertexArray(0);

        SE_TRACE("Renderer geometry initialized");
    }

    void Renderer::CleanupGeometry() {
        if (s_QuadVAO != 0) {
            glDeleteVertexArrays(1, &s_QuadVAO);
            glDeleteBuffers(1, &s_QuadVBO);
            s_QuadVAO = 0;
            s_QuadVBO = 0;
        }

        if (s_CubeVAO != 0) {
            glDeleteVertexArrays(1, &s_CubeVAO);
            glDeleteBuffers(1, &s_CubeVBO);
            s_CubeVAO = 0;
            s_CubeVBO = 0;
        }

        if (s_LineVAO != 0) {
            glDeleteVertexArrays(1, &s_LineVAO);
            glDeleteBuffers(1, &s_LineVBO);
            s_LineVAO = 0;
            s_LineVBO = 0;
        }

        if (s_TexturedQuadVAO != 0) {
            glDeleteVertexArrays(1, &s_TexturedQuadVAO);
            glDeleteBuffers(1, &s_TexturedQuadVBO);
            glDeleteBuffers(1, &s_TexturedQuadEBO);
            s_TexturedQuadVAO = 0;
            s_TexturedQuadVBO = 0;
            s_TexturedQuadEBO = 0;
        }

        SE_TRACE("Renderer geometry cleaned up");
    }

    void Renderer::UpdateShaderUniforms() {
        if (s_DefaultShader) {
            s_DefaultShader->SetMat4("u_ViewProjection", s_ProjectionMatrix * s_ViewMatrix);
            s_DefaultShader->SetMat4("u_Transform", s_ModelMatrix);
        }
    }

    bool ValidateOpenGLVersion(const char* versionString) {
        if (!versionString) {
            SE_ERROR("OpenGL version string is null");
            return false;
        }
        
        // Parse major.minor version from string like "4.6.0 NVIDIA 545.84"
        int major = 0, minor = 0;
        if (sscanf(versionString, "%d.%d", &major, &minor) != 2) {
            SE_ERROR("Failed to parse OpenGL version from: {}", versionString);
            return false;
        }
        
        // Require minimum OpenGL 3.3
        const int requiredMajor = 3, requiredMinor = 3;
        bool versionOk = (major > requiredMajor) || (major == requiredMajor && minor >= requiredMinor);
        
        if (!versionOk) {
            SE_ERROR("OpenGL {}.{} required, but only {}.{} available", 
                     requiredMajor, requiredMinor, major, minor);
            return false;
        }
        
        SE_INFO("OpenGL version validation passed: {}.{} >= {}.{}", 
                major, minor, requiredMajor, requiredMinor);
        return true;
    }

    void CheckGLErrorDebug(const char* operation, const char* file, int line) {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            const char* errorString = "Unknown";
            switch (error) {
                case GL_INVALID_ENUM: errorString = "GL_INVALID_ENUM"; break;
                case GL_INVALID_VALUE: errorString = "GL_INVALID_VALUE"; break;
                case GL_INVALID_OPERATION: errorString = "GL_INVALID_OPERATION"; break;
                case GL_OUT_OF_MEMORY: errorString = "GL_OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: errorString = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            SE_ERROR("OpenGL Error: {} in {} at {}:{}", errorString, operation, file, line);
        }
    }

} // namespace SilicaEngine