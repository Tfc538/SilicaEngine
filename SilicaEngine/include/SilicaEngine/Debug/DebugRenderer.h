/**
 * @file DebugRenderer.h
 * @brief Debug rendering module for development visualization
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Essential debug rendering functionality for development - wireframes, bounding boxes, debug text.
 */

#pragma once

#include "SilicaEngine/Core/Result.h"
#include "SilicaEngine/Renderer/Shader.h"
#include "SilicaEngine/Renderer/Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include <memory>
#include <mutex> // Added for mutex

namespace SilicaEngine {

    class Shader;

    /// Debug render mode for different visualization types
    enum class DebugRenderMode {
        Lines,          // Line segments
        Wireframe,      // Wireframe meshes
        Points,         // Point sprites
        Text,           // Debug text
        Solid           // Solid colored shapes
    };

    /// Debug line segment
    struct DebugLine {
        glm::vec3 start;
        glm::vec3 end;
        glm::vec4 color;
        float thickness = 1.0f;
        bool depthTest = true;
    };

    /// Debug point
    struct DebugPoint {
        glm::vec3 position;
        glm::vec4 color;
        float size = 5.0f;
        bool depthTest = true;
    };

    /// Debug text entry
    struct DebugText {
        std::string text;
        glm::vec3 worldPosition;
        glm::vec2 screenPosition;
        glm::vec4 color;
        float scale = 1.0f;
        bool useWorldPosition = true;
        bool depthTest = false;
    };

    /// Debug wireframe box
    struct DebugBox {
        glm::vec3 center;
        glm::vec3 size;
        glm::quat rotation = glm::quat(1, 0, 0, 0);
        glm::vec4 color;
        bool filled = false;
        bool depthTest = true;
    };

    /// Debug sphere
    struct DebugSphere {
        glm::vec3 center;
        float radius;
        glm::vec4 color;
        int segments = 16;
        bool filled = false;
        bool depthTest = true;
    };

    /// Debug rendering system for development visualization
    class DebugRenderer {
    public:
        /// Initialize the debug renderer
        static ErrorResult<void> Initialize();
        
        /// Shutdown the debug renderer
        static void Shutdown();
        
        /// Check if debug renderer is enabled
        static bool IsEnabled();
        
        /// Enable/disable debug rendering globally
        static void SetEnabled(bool enabled);
        
        /// Clear all debug primitives
        static void Clear();
        
        /// Render all debug primitives
        static void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        
        // === Line Rendering ===
        
        /// Draw a line between two points
        static void DrawLine(const glm::vec3& start, const glm::vec3& end, 
                            const glm::vec4& color = glm::vec4(1.0f), 
                            float thickness = 1.0f, bool depthTest = true);
        
        /// Draw multiple connected lines
        static void DrawPolyLine(const std::vector<glm::vec3>& points, 
                                const glm::vec4& color = glm::vec4(1.0f),
                                float thickness = 1.0f, bool depthTest = true);
        
        /// Draw coordinate axes at position
        static void DrawAxes(const glm::vec3& position, const glm::quat& rotation = glm::quat(1, 0, 0, 0),
                            float size = 1.0f, bool depthTest = true);
        
        /// Draw a grid in the XZ plane
        static void DrawGrid(const glm::vec3& center = glm::vec3(0.0f), 
                            float size = 10.0f, int divisions = 10,
                            const glm::vec4& color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
        
        // === Shape Rendering ===
        
        /// Draw wireframe or filled box
        static void DrawBox(const glm::vec3& center, const glm::vec3& size,
                           const glm::quat& rotation = glm::quat(1, 0, 0, 0),
                           const glm::vec4& color = glm::vec4(1.0f),
                           bool filled = false, bool depthTest = true);
        
        /// Draw wireframe or filled sphere
        static void DrawSphere(const glm::vec3& center, float radius,
                              const glm::vec4& color = glm::vec4(1.0f),
                              int segments = 16, bool filled = false, bool depthTest = true);
        
        /// Draw wireframe or filled cylinder
        static void DrawCylinder(const glm::vec3& start, const glm::vec3& end, float radius,
                                const glm::vec4& color = glm::vec4(1.0f),
                                int segments = 12, bool filled = false, bool depthTest = true);
        
        /// Draw a capsule (cylinder with spherical ends)
        static void DrawCapsule(const glm::vec3& start, const glm::vec3& end, float radius,
                               const glm::vec4& color = glm::vec4(1.0f),
                               int segments = 12, bool filled = false, bool depthTest = true);
        
        // === Utility Shapes ===
        
        /// Draw bounding box from min/max points
        static void DrawBoundingBox(const glm::vec3& min, const glm::vec3& max,
                                   const glm::vec4& color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
                                   bool depthTest = true);
        
        /// Draw frustum (e.g., camera frustum)
        static void DrawFrustum(const glm::mat4& viewProjectionMatrix,
                               const glm::vec4& color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
                               bool depthTest = true);
        
        /// Draw arrow from start to end
        static void DrawArrow(const glm::vec3& start, const glm::vec3& end,
                             const glm::vec4& color = glm::vec4(1.0f),
                             float arrowHeadSize = 0.1f, bool depthTest = true);
        
        // === Point Rendering ===
        
        /// Draw a point at position
        static void DrawPoint(const glm::vec3& position, 
                             const glm::vec4& color = glm::vec4(1.0f),
                             float size = 5.0f, bool depthTest = true);
        
        /// Draw multiple points
        static void DrawPoints(const std::vector<glm::vec3>& positions,
                              const glm::vec4& color = glm::vec4(1.0f),
                              float size = 5.0f, bool depthTest = true);
        
        // === Text Rendering ===
        
        /// Draw text at world position
        static void DrawTextWorld(const std::string& text, const glm::vec3& worldPosition,
                                 const glm::vec4& color = glm::vec4(1.0f),
                                 float scale = 1.0f, bool depthTest = false);
        
        /// Draw text at screen position (pixels)
        static void DrawTextScreen(const std::string& text, const glm::vec2& screenPosition,
                                  const glm::vec4& color = glm::vec4(1.0f),
                                  float scale = 1.0f);
        
        /// Draw formatted text at screen position
        static void DrawTextScreenf(const glm::vec2& screenPosition, const glm::vec4& color,
                                   float scale, const char* format, ...);
        
        // === Performance Monitoring ===
        
        /// Draw performance overlay
        static void DrawPerformanceOverlay(const glm::vec2& position = glm::vec2(10.0f, 10.0f));
        
        /// Draw memory usage overlay
        static void DrawMemoryOverlay(const glm::vec2& position = glm::vec2(10.0f, 100.0f));
        
        /// Draw render statistics overlay
        static void DrawRenderStatsOverlay(const glm::vec2& position = glm::vec2(10.0f, 200.0f));
        
        // === Settings ===
        
        /// Set default line thickness
        static void SetDefaultLineThickness(float thickness);
        
        /// Set default point size
        static void SetDefaultPointSize(float size);
        
        /// Set default text scale
        static void SetDefaultTextScale(float scale);
        
        /// Enable/disable depth testing for all debug primitives
        static void SetGlobalDepthTest(bool enabled);
        
        /// Get number of debug primitives queued for rendering
        static size_t GetPrimitiveCount();

    private:
        DebugRenderer() = default;
        ~DebugRenderer() = default;
        
        /// Internal storage for debug primitives
        static std::vector<DebugLine> s_lines;
        static std::vector<DebugPoint> s_points;
        static std::vector<DebugText> s_texts;
        static std::vector<DebugBox> s_boxes;
        static std::vector<DebugSphere> s_spheres;
        
        /// Thread safety for static containers
        static std::mutex s_Mutex;
        
        /// Rendering resources
        static std::shared_ptr<Shader> s_lineShader;
        static std::shared_ptr<Shader> s_pointShader;
        static std::shared_ptr<Shader> s_solidShader;
        static std::shared_ptr<Shader> s_textShader;
        
        /// OpenGL resources
        static uint32_t s_lineVAO, s_lineVBO;
        static uint32_t s_pointVAO, s_pointVBO;
        static uint32_t s_quadVAO, s_quadVBO;
        
        /// Settings
        static bool s_enabled;
        static bool s_initialized;
        static float s_defaultLineThickness;
        static float s_defaultPointSize;
        static float s_defaultTextScale;
        static bool s_globalDepthTest;
        
        /// Internal rendering methods
        static void RenderLines(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        static void RenderPoints(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        static void RenderBoxes(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        static void RenderSpheres(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        static void RenderText(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        
        /// Generate geometry for shapes
        static void GenerateBoxGeometry(std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices, bool filled = false);
        static void GenerateSphereGeometry(std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices, int segments = 16, bool filled = false);
        
        /// Create default shaders
        static bool CreateShaders();
        static void DestroyShaders();
        
        /// Create OpenGL resources
        static bool CreateGeometry();
        static void DestroyGeometry();
    };

    /// Convenience macros for debug rendering
    #ifdef SILICA_DEBUG
        #define SE_DEBUG_LINE(start, end, color) SilicaEngine::DebugRenderer::DrawLine(start, end, color)
        #define SE_DEBUG_BOX(center, size, color) SilicaEngine::DebugRenderer::DrawBox(center, size, glm::quat(1,0,0,0), color)
        #define SE_DEBUG_SPHERE(center, radius, color) SilicaEngine::DebugRenderer::DrawSphere(center, radius, color)
        #define SE_DEBUG_POINT(pos, color) SilicaEngine::DebugRenderer::DrawPoint(pos, color)
        #define SE_DEBUG_TEXT(text, pos, color) SilicaEngine::DebugRenderer::DrawTextWorld(text, pos, color)
        #define SE_DEBUG_AXES(pos, rot, size) SilicaEngine::DebugRenderer::DrawAxes(pos, rot, size)
        #define SE_DEBUG_GRID() SilicaEngine::DebugRenderer::DrawGrid()
    #else
        #define SE_DEBUG_LINE(start, end, color) do {} while(0)
        #define SE_DEBUG_BOX(center, size, color) do {} while(0)
        #define SE_DEBUG_SPHERE(center, radius, color) do {} while(0)
        #define SE_DEBUG_POINT(pos, color) do {} while(0)
        #define SE_DEBUG_TEXT(text, pos, color) do {} while(0)
        #define SE_DEBUG_AXES(pos, rot, size) do {} while(0)
        #define SE_DEBUG_GRID() do {} while(0)
    #endif

} // namespace SilicaEngine