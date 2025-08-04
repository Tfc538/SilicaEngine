/**
 * @file ResourceTracker.h
 * @brief OpenGL resource cleanup validation for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Debug-only resource tracking to detect OpenGL resource leaks.
 */

#pragma once

#include <glad/gl.h>
#include <unordered_map>
#include <string>
#include <mutex>

namespace SilicaEngine {

    /// Debug resource tracker for OpenGL objects
    class ResourceTracker {
    public:
        /// Track creation of OpenGL resource
        static void TrackResource(uint32_t id, const std::string& type, const std::string& location);
        
        /// Track deletion of OpenGL resource
        static void UntrackResource(uint32_t id, const std::string& type);
        
        /// Report any leaked resources (call during shutdown)
        static void ReportLeaks();
        
        /// Clear all tracked resources (for testing)
        static void Clear();
        
    private:
        struct ResourceInfo {
            std::string type;
            std::string location;
            
            ResourceInfo(const std::string& t, const std::string& l) : type(t), location(l) {}
        };
        
        static std::unordered_map<uint32_t, ResourceInfo> s_TrackedResources;
        static std::mutex s_Mutex;
    };

} // namespace SilicaEngine

// Resource tracking macros (debug builds only)
#ifdef SILICA_DEBUG
    #define SE_TRACK_RESOURCE(id, type) \
        SilicaEngine::ResourceTracker::TrackResource(id, type, std::string(__FILE__) + ":" + std::to_string(__LINE__))
    #define SE_UNTRACK_RESOURCE(id, type) \
        SilicaEngine::ResourceTracker::UntrackResource(id, type)
#else
    #define SE_TRACK_RESOURCE(id, type)
    #define SE_UNTRACK_RESOURCE(id, type)
#endif