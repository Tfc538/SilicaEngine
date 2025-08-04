/**
 * @file ResourceHandle.cpp
 * @brief Implementation of stable resource handle system
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Core/ResourceHandle.h"
#include "SilicaEngine/Core/Logger.h"
#include <chrono>

namespace SilicaEngine {

    ResourceRegistry& ResourceRegistry::GetInstance() {
        static ResourceRegistry instance;
        return instance;
    }
    
    ResourceID ResourceRegistry::GenerateID() {
        return m_nextID.fetch_add(1, std::memory_order_relaxed);
    }
    
    void ResourceRegistry::Clear() {
        std::lock_guard<std::mutex> lock(m_registryMutex);
        SE_INFO("Clearing resource registry with {} resources", m_resources.size());
        m_resources.clear();
        m_nextID.store(1, std::memory_order_relaxed);
    }
    
    size_t ResourceRegistry::GetResourceCount(ResourceType type) {
        std::lock_guard<std::mutex> lock(m_registryMutex);
        
        size_t count = 0;
        for (const auto& [id, entry] : m_resources) {
            if (entry.type == type) {
                count++;
            }
        }
        
        return count;
    }
    
    size_t ResourceRegistry::GetTotalResourceCount() {
        std::lock_guard<std::mutex> lock(m_registryMutex);
        return m_resources.size();
    }

} // namespace SilicaEngine