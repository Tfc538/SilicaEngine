/**
 * @file BatchRenderer.cpp
 * @brief Batch rendering system implementation
 * @author Tim Gatzke <post@tim-gatzke.de>
 */

#include "SilicaEngine/Renderer/BatchRenderer.h"
#include "SilicaEngine/Renderer/Shader.h"
#include "SilicaEngine/Core/ErrorCodes.h"
#include "SilicaEngine/SilicaEngine.h"
#include <glm/gtc/matrix_transform.hpp>

namespace SilicaEngine {

    BatchRenderer::BatchRenderer()
        : m_InstanceVBO(0)
        , m_CubeVAO(0)
        , m_CubeVBO(0)
        , m_Initialized(false) {
    }

    BatchRenderer::~BatchRenderer() {
        Shutdown();
    }

    ErrorResult<void> BatchRenderer::Initialize() {
        if (m_Initialized) {
            return ErrorResult<void>::Success();
        }

        SE_INFO("Initializing batch renderer...");

        // Initialize shader first
        auto shaderResult = InitializeShader();
        if (!shaderResult) {
            SE_ERROR("Failed to initialize batch renderer shader: {}", shaderResult.errorMessage);
            return shaderResult;
        }

        // Initialize geometry
        auto geometryResult = InitializeGeometry();
        if (!geometryResult) {
            SE_ERROR("Failed to initialize batch renderer geometry: {}", geometryResult.errorMessage);
            return geometryResult;
        }

        m_Initialized = true;
        SE_INFO("Batch renderer initialized successfully");
        return ErrorResult<void>::Success();
    }

    ErrorResult<void> BatchRenderer::Shutdown() {
        if (!m_Initialized) {
            return ErrorResult<void>::Success();
        }

        SE_INFO("Shutting down batch renderer...");
        Cleanup();
        m_Initialized = false;
        return ErrorResult<void>::Success();
    }

    void BatchRenderer::AddCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color) {
        if (!m_Initialized) {
            SE_WARN("Batch renderer not initialized, cannot add cube");
            return;
        }

        // Check if batch is full
        if (m_Instances.size() >= MAX_BATCH_SIZE) {
            FlushBatch();
        }

        // Create instance data
        CubeInstance instance;
        instance.transform = glm::translate(glm::mat4(1.0f), position);
        instance.transform = glm::scale(instance.transform, size);
        instance.color = color;

        m_Instances.push_back(instance);
    }

    void BatchRenderer::FlushBatch() {
        if (!m_Initialized || m_Instances.empty()) {
            return;
        }

        // Bind shader and set view-projection matrix
        m_InstancedShader->Bind();
        m_InstancedShader->SetMat4("u_ViewProjection", Renderer::GetViewProjectionMatrix());

        // Bind cube VAO
        glBindVertexArray(m_CubeVAO);

        // Upload instance data to GPU
        glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 
                       m_Instances.size() * sizeof(CubeInstance), 
                       m_Instances.data());

        // Draw all instances with a single call
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, static_cast<GLsizei>(m_Instances.size()));

        // Update renderer stats
        Renderer::UpdateStats(1, static_cast<uint32_t>(36 * m_Instances.size()), static_cast<uint32_t>(12 * m_Instances.size()));

        // Clear batch
        m_Instances.clear();
    }

    ErrorResult<void> BatchRenderer::InitializeShader() {
        // Vertex shader with instancing support
        const char* vertexSource = R"(
            #version 460 core
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec4 a_Color;
            
            // Instance attributes
            layout (location = 2) in mat4 a_Transform;
            layout (location = 6) in vec4 a_InstanceColor;
            
            uniform mat4 u_ViewProjection;
            
            out vec4 v_Color;
            
            void main() {
                v_Color = a_InstanceColor;
                gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);
            }
        )";

        // Fragment shader
        const char* fragmentSource = R"(
            #version 460 core
            in vec4 v_Color;
            out vec4 FragColor;
            
            void main() {
                FragColor = v_Color;
            }
        )";

        m_InstancedShader = std::make_shared<Shader>();
        auto result = m_InstancedShader->CreateFromString(vertexSource, fragmentSource);
        
        if (!result) {
            SE_ERROR("Failed to create instanced shader: {}", result.errorMessage);
            return ErrorResult<void>::Error(EngineError::ShaderCompilationFailed, 
                                          "Failed to create instanced shader: " + result.errorMessage);
        }

        return ErrorResult<void>::Success();
    }

    ErrorResult<void> BatchRenderer::InitializeGeometry() {
        // Cube vertices (same as regular renderer but without colors since we use instance colors)
        float cubeVertices[] = {
            // positions
            -0.5f,-0.5f, 0.5f,  // bottom-left
             0.5f,-0.5f, 0.5f,  // bottom-right
             0.5f, 0.5f, 0.5f,  // top-right
            -0.5f,-0.5f, 0.5f,  // bottom-left
             0.5f, 0.5f, 0.5f,  // top-right
            -0.5f, 0.5f, 0.5f,  // top-left
        
            // Back
             0.5f,-0.5f,-0.5f,  // bottom-left
            -0.5f,-0.5f,-0.5f,  // bottom-right
            -0.5f, 0.5f,-0.5f,  // top-right
             0.5f,-0.5f,-0.5f,  // bottom-left
            -0.5f, 0.5f,-0.5f,  // top-right
             0.5f, 0.5f,-0.5f,  // top-left
        
            // Left
            -0.5f,-0.5f,-0.5f,  // bottom-left
            -0.5f,-0.5f, 0.5f,  // bottom-right
            -0.5f, 0.5f, 0.5f,  // top-right
            -0.5f,-0.5f,-0.5f,  // bottom-left
            -0.5f, 0.5f, 0.5f,  // top-right
            -0.5f, 0.5f,-0.5f,  // top-left
        
            // Right
             0.5f,-0.5f, 0.5f,  // bottom-left
             0.5f,-0.5f,-0.5f,  // bottom-right
             0.5f, 0.5f,-0.5f,  // top-right
             0.5f,-0.5f, 0.5f,  // bottom-left
             0.5f, 0.5f,-0.5f,  // top-right
             0.5f, 0.5f, 0.5f,  // top-left
        
            // Top
            -0.5f, 0.5f, 0.5f,  // bottom-left
             0.5f, 0.5f, 0.5f,  // bottom-right
             0.5f, 0.5f,-0.5f,  // top-right
            -0.5f, 0.5f, 0.5f,  // bottom-left
             0.5f, 0.5f,-0.5f,  // top-right
            -0.5f, 0.5f,-0.5f,  // top-left
        
            // Bottom
            -0.5f,-0.5f,-0.5f,  // bottom-left
             0.5f,-0.5f,-0.5f,  // bottom-right
             0.5f,-0.5f, 0.5f,  // top-right
            -0.5f,-0.5f,-0.5f,  // bottom-left
             0.5f,-0.5f, 0.5f,  // top-right
            -0.5f,-0.5f, 0.5f   // top-left
        };

        // Create cube VAO and VBO
        glGenVertexArrays(1, &m_CubeVAO);
        glGenBuffers(1, &m_CubeVBO);

        glBindVertexArray(m_CubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Create instance VBO
        glGenBuffers(1, &m_InstanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SIZE * sizeof(CubeInstance), nullptr, GL_DYNAMIC_DRAW);

        // Instance transform matrix (4 vec4s)
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CubeInstance), (void*)offsetof(CubeInstance, transform));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(CubeInstance), (void*)(offsetof(CubeInstance, transform) + sizeof(glm::vec4)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(CubeInstance), (void*)(offsetof(CubeInstance, transform) + 2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(CubeInstance), (void*)(offsetof(CubeInstance, transform) + 3 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);

        // Instance color
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(CubeInstance), (void*)offsetof(CubeInstance, color));
        glEnableVertexAttribArray(6);

        // Set instance divisors
        glVertexAttribDivisor(2, 1); // Transform matrix
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1); // Color

        return ErrorResult<void>::Success();
    }

    void BatchRenderer::Cleanup() {
        if (m_InstanceVBO != 0) {
            glDeleteBuffers(1, &m_InstanceVBO);
            m_InstanceVBO = 0;
        }
        
        if (m_CubeVBO != 0) {
            glDeleteBuffers(1, &m_CubeVBO);
            m_CubeVBO = 0;
        }
        
        if (m_CubeVAO != 0) {
            glDeleteVertexArrays(1, &m_CubeVAO);
            m_CubeVAO = 0;
        }

        m_InstancedShader.reset();
    }

} // namespace SilicaEngine 