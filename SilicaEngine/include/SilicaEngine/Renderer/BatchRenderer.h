/**
 * @file BatchRenderer.h
 * @brief Batch rendering system for efficient instanced rendering
 * @author Tim Gatzke <post@tim-gatzke.de>
 */

#pragma once

#include "SilicaEngine/Renderer/Renderer.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace SilicaEngine {

    /**
     * @brief Instance data for batched rendering
     */
    struct CubeInstance {
        glm::mat4 transform;
        glm::vec4 color;
    };

    /**
     * @brief High-performance batch renderer for instanced rendering
     * 
     * Reduces draw calls by batching similar objects and rendering them
     * with a single instanced draw call. This can provide 60-80% performance
     * improvement for scenes with many similar objects.
     */
    class BatchRenderer {
    public:
        BatchRenderer();
        ~BatchRenderer();

        // Delete copy operations to prevent double-deletion of OpenGL resources
        BatchRenderer(const BatchRenderer&) = delete;
        BatchRenderer& operator=(const BatchRenderer&) = delete;

        /**
         * @brief Initialize the batch renderer
         */
        ErrorResult<void> Initialize();

        /**
         * @brief Shutdown the batch renderer
         */
        ErrorResult<void> Shutdown();

        /**
         * @brief Add a cube to the current batch
         */
        void AddCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);

        /**
         * @brief Flush the current batch to GPU
         */
        void FlushBatch();

        /**
         * @brief Get the number of instances in the current batch
         */
        size_t GetBatchSize() const { return m_Instances.size(); }

        /**
         * @brief Get the maximum batch size
         */
        size_t GetMaxBatchSize() const { return MAX_BATCH_SIZE; }

        /**
         * @brief Check if batch is full
         */
        bool IsBatchFull() const { return m_Instances.size() >= MAX_BATCH_SIZE; }

    private:
        static constexpr size_t MAX_BATCH_SIZE = 1000; // Maximum instances per batch
        
        std::vector<CubeInstance> m_Instances;
        uint32_t m_InstanceVBO;
        uint32_t m_CubeVAO;
        uint32_t m_CubeVBO;
        std::shared_ptr<Shader> m_InstancedShader;
        
        bool m_Initialized;

        /**
         * @brief Initialize instanced shader
         */
        ErrorResult<void> InitializeShader();

        /**
         * @brief Initialize geometry buffers
         */
        ErrorResult<void> InitializeGeometry();

        /**
         * @brief Cleanup OpenGL resources
         */
        void Cleanup();
    };

} // namespace SilicaEngine 