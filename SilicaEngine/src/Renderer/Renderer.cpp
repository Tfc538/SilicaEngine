/**
 * @file Renderer.cpp
 * @brief Implementation of the Renderer class for SilicaEngine
 */

#include "SilicaEngine/Renderer/Renderer.h"
#include "SilicaEngine/Renderer/Shader.h"
#include "SilicaEngine/SilicaEngine.h"  // For logging macros
#include <glm/gtc/matrix_transform.hpp>

namespace SilicaEngine {

    // Static member definitions
    bool Renderer::s_Initialized = false;
    RendererStats Renderer::s_Stats;
    RenderState Renderer::s_RenderState;
    glm::mat4 Renderer::s_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 Renderer::s_ProjectionMatrix = glm::mat4(1.0f);
    glm::mat4 Renderer::s_ModelMatrix = glm::mat4(1.0f);
    std::shared_ptr<Shader> Renderer::s_DefaultShader;
    
    // Geometry VAOs and VBOs
    uint32_t Renderer::s_QuadVAO = 0;
    uint32_t Renderer::s_QuadVBO = 0;
    uint32_t Renderer::s_CubeVAO = 0;
    uint32_t Renderer::s_CubeVBO = 0;
    uint32_t Renderer::s_LineVAO = 0;
    uint32_t Renderer::s_LineVBO = 0;

    bool Renderer::Initialize() {
        if (s_Initialized) {
            return true;
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

        // Create default shader
        s_DefaultShader = Shader::CreateDefault();
        if (!s_DefaultShader) {
            SE_ERROR("Failed to create default shader");
            return false;
        }

        // Initialize geometry
        InitializeGeometry();

        // Set default render state
        s_RenderState = RenderState{};
        ApplyRenderState(s_RenderState);

        // Set default matrices
        s_ViewMatrix = glm::mat4(1.0f);
        s_ProjectionMatrix = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
        s_ModelMatrix = glm::mat4(1.0f);

        s_Initialized = true;
        SE_INFO("Renderer initialized successfully");
        return true;
    }

    void Renderer::Shutdown() {
        if (!s_Initialized) {
            return;
        }

        SE_INFO("Shutting down renderer...");

        // Cleanup geometry
        CleanupGeometry();

        // Release default shader
        s_DefaultShader.reset();

        s_Initialized = false;
        SE_INFO("Renderer shutdown complete");
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

    const RendererStats& Renderer::GetStats() {
        return s_Stats;
    }

    void Renderer::ResetStats() {
        s_Stats.Reset();
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

        SE_TRACE("Renderer geometry cleaned up");
    }

    void Renderer::UpdateShaderUniforms() {
        if (s_DefaultShader) {
            s_DefaultShader->SetMat4("u_ViewProjection", s_ProjectionMatrix * s_ViewMatrix);
            s_DefaultShader->SetMat4("u_Transform", s_ModelMatrix);
        }
    }

} // namespace SilicaEngine