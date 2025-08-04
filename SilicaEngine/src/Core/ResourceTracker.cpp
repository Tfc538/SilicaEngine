/**
 * @file ResourceTracker.cpp
 * @brief Implementation of OpenGL resource cleanup validation
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Core/ResourceTracker.h"
#include "SilicaEngine/Core/Logger.h"
#include <unordered_map>
#include <mutex>

namespace SilicaEngine {

    std::unordered_map<uint32_t, ResourceTracker::ResourceInfo> ResourceTracker::s_TrackedResources;
    std::mutex ResourceTracker::s_Mutex;

    void ResourceTracker::TrackResource(uint32_t id, const std::string& type, const std::string& location) {
        if (id == 0) return; // Invalid OpenGL ID
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_TrackedResources.emplace(id, ResourceInfo(type, location));
        SE_TRACE("Tracking {} resource: {} at {}", type, id, location);
    }

    void ResourceTracker::UntrackResource(uint32_t id, const std::string& type) {
        if (id == 0) return; // Invalid OpenGL ID
        
        std::lock_guard<std::mutex> lock(s_Mutex);
        auto it = s_TrackedResources.find(id);
        if (it != s_TrackedResources.end()) {
            SE_TRACE("Untracking {} resource: {}", type, id);
            s_TrackedResources.erase(it);
        } else {
            SE_WARN("Attempted to untrack unknown {} resource: {}", type, id);
        }
    }

    void ResourceTracker::ReportLeaks() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        if (s_TrackedResources.empty()) {
            SE_INFO("Resource tracking: No leaks detected");
            return;
        }
        
        SE_ERROR("Resource tracking: {} leaked resources detected:", s_TrackedResources.size());
        for (const auto& [id, info] : s_TrackedResources) {
            SE_ERROR("  {} {} leaked - created at {}", info.type, id, info.location);
        }
    }

    void ResourceTracker::Clear() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_TrackedResources.clear();
    }

} // namespace SilicaEngine