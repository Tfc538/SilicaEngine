/**
 * @file DebugRenderer.cpp
 * @brief Implementation of debug rendering module
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Debug/DebugRenderer.h"
#include "SilicaEngine/Renderer/Shader.h"
#include "SilicaEngine/Core/Logger.h"
#include <glad/gl.h>
#include <cstdarg>
#include <cstdio>
#include <mutex> // Added for mutex protection

namespace SilicaEngine {

    // Static member definitions
    std::vector<DebugLine> DebugRenderer::s_lines;
    std::vector<DebugPoint> DebugRenderer::s_points;
    std::vector<DebugText> DebugRenderer::s_texts;
    std::vector<DebugBox> DebugRenderer::s_boxes;
    std::vector<DebugSphere> DebugRenderer::s_spheres;
    
    std::shared_ptr<Shader> DebugRenderer::s_lineShader;
    std::shared_ptr<Shader> DebugRenderer::s_pointShader;
    std::shared_ptr<Shader> DebugRenderer::s_solidShader;
    std::shared_ptr<Shader> DebugRenderer::s_textShader;
    
    uint32_t DebugRenderer::s_lineVAO = 0;
    uint32_t DebugRenderer::s_lineVBO = 0;
    uint32_t DebugRenderer::s_pointVAO = 0;
    uint32_t DebugRenderer::s_pointVBO = 0;
    uint32_t DebugRenderer::s_quadVAO = 0;
    uint32_t DebugRenderer::s_quadVBO = 0;
    
    bool DebugRenderer::s_enabled = true;
    bool DebugRenderer::s_initialized = false;
    float DebugRenderer::s_defaultLineThickness = 1.0f;
    float DebugRenderer::s_defaultPointSize = 5.0f;
    float DebugRenderer::s_defaultTextScale = 1.0f;
    bool DebugRenderer::s_globalDepthTest = true;

    std::mutex DebugRenderer::s_Mutex; // Added mutex for static containers

    ErrorResult<void> DebugRenderer::Initialize() {
        if (s_initialized) {
            SE_WARN("DebugRenderer already initialized");
            return ErrorResult<void>::Success();
        }
        
        SE_INFO("Initializing DebugRenderer");
        
        if (!CreateShaders()) {
            SE_ERROR("Failed to create debug shaders");
            return ErrorResult<void>::Error(EngineError::InitializationFailed, "Failed to create debug shaders");
        }
        
        if (!CreateGeometry()) {
            SE_ERROR("Failed to create debug geometry");
            DestroyShaders();
            return ErrorResult<void>::Error(EngineError::InitializationFailed, "Failed to create debug geometry");
        }
        
        s_initialized = true;
        SE_INFO("DebugRenderer initialized successfully");
        return ErrorResult<void>::Success();
    }
    
    void DebugRenderer::Shutdown() {
        if (!s_initialized) {
            return;
        }
        
        SE_INFO("Shutting down DebugRenderer");
        
        Clear();
        DestroyGeometry();
        DestroyShaders();
        
        s_initialized = false;
        SE_INFO("DebugRenderer shutdown complete");
    }
    
    bool DebugRenderer::IsEnabled() {
        return s_enabled && s_initialized;
    }
    
    void DebugRenderer::SetEnabled(bool enabled) {
        s_enabled = enabled;
    }
    
    void DebugRenderer::Clear() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_lines.clear();
        s_points.clear();
        s_texts.clear();
        s_boxes.clear();
        s_spheres.clear();
    }
    
    void DebugRenderer::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        if (!IsEnabled()) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        // Store OpenGL state
        GLboolean depthTestEnabled;
        glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
        
        GLboolean blendEnabled;
        glGetBooleanv(GL_BLEND, &blendEnabled);
        
        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Render all debug primitives
        RenderLines(viewMatrix, projectionMatrix);
        RenderPoints(viewMatrix, projectionMatrix);
        RenderBoxes(viewMatrix, projectionMatrix);
        RenderSpheres(viewMatrix, projectionMatrix);
        RenderText(viewMatrix, projectionMatrix);
        
        // Restore OpenGL state
        if (!depthTestEnabled) glDisable(GL_DEPTH_TEST);
        if (!blendEnabled) glDisable(GL_BLEND);
    }
    
    void DebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, 
                                const glm::vec4& color, float thickness, bool depthTest) {
        if (!IsEnabled()) return;
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        DebugLine line;
        line.start = start;
        line.end = end;
        line.color = color;
        line.thickness = thickness > 0.0f ? thickness : s_defaultLineThickness;
        line.depthTest = depthTest;
        
        s_lines.push_back(line);
    }
    
    void DebugRenderer::DrawPolyLine(const std::vector<glm::vec3>& points, 
                                    const glm::vec4& color, float thickness, bool depthTest) {
        if (!IsEnabled() || points.size() < 2) return;
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        for (size_t i = 0; i < points.size() - 1; ++i) {
            DebugLine line;
            line.start = points[i];
            line.end = points[i + 1];
            line.color = color;
            line.thickness = thickness > 0.0f ? thickness : s_defaultLineThickness;
            line.depthTest = depthTest;
            
            s_lines.push_back(line);
        }
    }
    
    void DebugRenderer::DrawAxes(const glm::vec3& position, const glm::quat& rotation, 
                                float size, bool depthTest) {
        if (!IsEnabled()) return;
        
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation);
        
        glm::vec3 xAxis = transform * glm::vec4(size, 0, 0, 0);
        glm::vec3 yAxis = transform * glm::vec4(0, size, 0, 0);
        glm::vec3 zAxis = transform * glm::vec4(0, 0, size, 0);
        
        DrawLine(position, position + xAxis, glm::vec4(1, 0, 0, 1), s_defaultLineThickness, depthTest);
        DrawLine(position, position + yAxis, glm::vec4(0, 1, 0, 1), s_defaultLineThickness, depthTest);
        DrawLine(position, position + zAxis, glm::vec4(0, 0, 1, 1), s_defaultLineThickness, depthTest);
    }
    
    void DebugRenderer::DrawGrid(const glm::vec3& center, float size, int divisions, 
                                const glm::vec4& color) {
        if (!IsEnabled()) return;
        
        float stepSize = size / divisions;
        float halfSize = size * 0.5f;
        
        // Draw lines parallel to X axis
        for (int i = 0; i <= divisions; ++i) {
            float z = -halfSize + i * stepSize;
            glm::vec3 start = center + glm::vec3(-halfSize, 0, z);
            glm::vec3 end = center + glm::vec3(halfSize, 0, z);
            DrawLine(start, end, color, s_defaultLineThickness, s_globalDepthTest);
        }
        
        // Draw lines parallel to Z axis
        for (int i = 0; i <= divisions; ++i) {
            float x = -halfSize + i * stepSize;
            glm::vec3 start = center + glm::vec3(x, 0, -halfSize);
            glm::vec3 end = center + glm::vec3(x, 0, halfSize);
            DrawLine(start, end, color, s_defaultLineThickness, s_globalDepthTest);
        }
    }
    
    void DebugRenderer::DrawBox(const glm::vec3& center, const glm::vec3& size,
                               const glm::quat& rotation, const glm::vec4& color,
                               bool filled, bool depthTest) {
        if (!IsEnabled()) return;
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        DebugBox box;
        box.center = center;
        box.size = size;
        box.rotation = rotation;
        box.color = color;
        box.filled = filled;
        box.depthTest = depthTest;
        
        s_boxes.push_back(box);
    }
    
    void DebugRenderer::DrawSphere(const glm::vec3& center, float radius,
                                  const glm::vec4& color, int segments, bool filled, bool depthTest) {
        if (!IsEnabled()) return;
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        DebugSphere sphere;
        sphere.center = center;
        sphere.radius = radius;
        sphere.color = color;
        sphere.segments = segments;
        sphere.filled = filled;
        sphere.depthTest = depthTest;
        
        s_spheres.push_back(sphere);
    }
    
    void DebugRenderer::DrawBoundingBox(const glm::vec3& min, const glm::vec3& max,
                                       const glm::vec4& color, bool depthTest) {
        glm::vec3 center = (min + max) * 0.5f;
        glm::vec3 size = max - min;
        DrawBox(center, size, glm::quat(1, 0, 0, 0), color, false, depthTest);
    }
    
    void DebugRenderer::DrawPoint(const glm::vec3& position, 
                                 const glm::vec4& color, float size, bool depthTest) {
        if (!IsEnabled()) return;
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        DebugPoint point;
        point.position = position;
        point.color = color;
        point.size = size > 0.0f ? size : s_defaultPointSize;
        point.depthTest = depthTest;
        
        s_points.push_back(point);
    }
    
    void DebugRenderer::DrawTextWorld(const std::string& text, const glm::vec3& worldPosition,
                                     const glm::vec4& color, float scale, bool depthTest) {
        if (!IsEnabled()) return;
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        DebugText debugText;
        debugText.text = text;
        debugText.worldPosition = worldPosition;
        debugText.color = color;
        debugText.scale = scale > 0.0f ? scale : s_defaultTextScale;
        debugText.useWorldPosition = true;
        debugText.depthTest = depthTest;
        
        s_texts.push_back(debugText);
    }
    
    void DebugRenderer::DrawTextScreen(const std::string& text, const glm::vec2& screenPosition,
                                      const glm::vec4& color, float scale) {
        if (!IsEnabled()) return;
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        DebugText debugText;
        debugText.text = text;
        debugText.screenPosition = screenPosition;
        debugText.color = color;
        debugText.scale = scale > 0.0f ? scale : s_defaultTextScale;
        debugText.useWorldPosition = false;
        debugText.depthTest = false;
        
        s_texts.push_back(debugText);
    }
    
    size_t DebugRenderer::GetPrimitiveCount() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        return s_lines.size() + s_points.size() + s_texts.size() + s_boxes.size() + s_spheres.size();
    }
    
    // === Internal Implementation ===
    
    void DebugRenderer::RenderLines(const glm::mat4& /*viewMatrix*/, const glm::mat4& /*projectionMatrix*/) {
        if (s_lines.empty() || !s_lineShader) return;
        
        // TODO: Implement line rendering
        // This would involve:
        // 1. Upload line data to VBO
        // 2. Set appropriate GL state (line width, depth test)
        // 3. Render with line shader
    }
    
    void DebugRenderer::RenderPoints(const glm::mat4& /*viewMatrix*/, const glm::mat4& /*projectionMatrix*/) {
        if (s_points.empty() || !s_pointShader) return;
        
        // TODO: Implement point rendering
    }
    
    void DebugRenderer::RenderBoxes(const glm::mat4& /*viewMatrix*/, const glm::mat4& /*projectionMatrix*/) {
        if (s_boxes.empty() || !s_lineShader) return;
        
        // TODO: Implement box rendering
    }
    
    void DebugRenderer::RenderSpheres(const glm::mat4& /*viewMatrix*/, const glm::mat4& /*projectionMatrix*/) {
        if (s_spheres.empty() || !s_lineShader) return;
        
        // TODO: Implement sphere rendering
    }
    
    void DebugRenderer::RenderText(const glm::mat4& /*viewMatrix*/, const glm::mat4& /*projectionMatrix*/) {
        if (s_texts.empty() || !s_textShader) return;
        
        // TODO: Implement text rendering
    }
    
    bool DebugRenderer::CreateShaders() {
        // Simple line shader
        const char* lineVertexShader = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec4 aColor;
            
            uniform mat4 uViewProjection;
            
            out vec4 vColor;
            
            void main() {
                gl_Position = uViewProjection * vec4(aPos, 1.0);
                vColor = aColor;
            }
        )";
        
        const char* lineFragmentShader = R"(
            #version 330 core
            in vec4 vColor;
            out vec4 FragColor;
            
            void main() {
                FragColor = vColor;
            }
        )";
        // Point shader (point size)
        const char* pointVertexShader = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec4 aColor;
            uniform mat4 uViewProjection;
            uniform float uPointSize;
            out vec4 vColor;
            void main() {
                gl_Position = uViewProjection * vec4(aPos, 1.0);
                gl_PointSize = uPointSize;
                vColor = aColor;
            }
        )";
        const char* pointFragmentShader = R"(
            #version 330 core
            in vec4 vColor;
            out vec4 FragColor;
            void main() {
                FragColor = vColor;
            }
        )";
        // Solid shader (for filled primitives)
        const char* solidVertexShader = lineVertexShader;
        const char* solidFragmentShader = lineFragmentShader;
        // Text shader (texture sampling)
        const char* textVertexShader = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec2 aUV;
            layout (location = 2) in vec4 aColor;
            uniform mat4 uViewProjection;
            out vec2 vUV;
            out vec4 vColor;
            void main() {
                gl_Position = uViewProjection * vec4(aPos, 1.0);
                vUV = aUV;
                vColor = aColor;
            }
        )";
        const char* textFragmentShader = R"(
            #version 330 core
            in vec2 vUV;
            in vec4 vColor;
            uniform sampler2D uFontTexture;
            out vec4 FragColor;
            void main() {
                float alpha = texture(uFontTexture, vUV).r;
                FragColor = vec4(vColor.rgb, vColor.a * alpha);
            }
        )";
        // Create shaders
        auto lineShader = std::make_shared<Shader>();
        if (lineShader->CreateFromString(lineVertexShader, lineFragmentShader, "")) {
            s_lineShader = lineShader;
        }
        auto pointShader = std::make_shared<Shader>();
        if (pointShader->CreateFromString(pointVertexShader, pointFragmentShader, "")) {
            s_pointShader = pointShader;
        }
        auto solidShader = std::make_shared<Shader>();
        if (solidShader->CreateFromString(solidVertexShader, solidFragmentShader, "")) {
            s_solidShader = solidShader;
        }
        auto textShader = std::make_shared<Shader>();
        if (textShader->CreateFromString(textVertexShader, textFragmentShader, "")) {
            s_textShader = textShader;
        }
        if (!s_lineShader || !s_pointShader || !s_solidShader || !s_textShader) {
            SE_ERROR("Failed to create one or more debug shaders");
            return false;
        }
        return true;
    }
    
    void DebugRenderer::DestroyShaders() {
        s_lineShader.reset();
        s_pointShader.reset();
        s_solidShader.reset();
        s_textShader.reset();
    }
    
    bool DebugRenderer::CreateGeometry() {
        // Create line VAO/VBO
        glGenVertexArrays(1, &s_lineVAO);
        glGenBuffers(1, &s_lineVBO);
        
        glBindVertexArray(s_lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_lineVBO);
        
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
        
        // Create point VAO/VBO (similar setup)
        glGenVertexArrays(1, &s_pointVAO);
        glGenBuffers(1, &s_pointVBO);
        
        // Create quad VAO/VBO for text rendering
        glGenVertexArrays(1, &s_quadVAO);
        glGenBuffers(1, &s_quadVBO);
        
        return true;
    }
    
    void DebugRenderer::DestroyGeometry() {
        if (s_lineVAO) {
            glDeleteVertexArrays(1, &s_lineVAO);
            glDeleteBuffers(1, &s_lineVBO);
            s_lineVAO = s_lineVBO = 0;
        }
        
        if (s_pointVAO) {
            glDeleteVertexArrays(1, &s_pointVAO);
            glDeleteBuffers(1, &s_pointVBO);
            s_pointVAO = s_pointVBO = 0;
        }
        
        if (s_quadVAO) {
            glDeleteVertexArrays(1, &s_quadVAO);
            glDeleteBuffers(1, &s_quadVBO);
            s_quadVAO = s_quadVBO = 0;
        }
    }

} // namespace SilicaEngine