/**
 * @file ResourceHandle.h
 * @brief Stable resource handle system for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Provides stable handles to resources that remain valid across reloads and memory changes.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <string>
#include <chrono>
#include <vector>

namespace SilicaEngine {

    /// Resource type identifier
    enum class ResourceType : uint16_t {
        Unknown = 0,
        Texture = 1,
        Shader = 2,
        Mesh = 3,
        Audio = 4,
        Material = 5,
        Animation = 6,
        Font = 7
    };

    /// Unique resource handle ID
    using ResourceID = uint64_t;
    
    /// Invalid/null resource handle
    static constexpr ResourceID INVALID_RESOURCE_ID = 0;

    /// Resource handle that provides stable access to resources
    template<typename T>
    class ResourceHandle {
    public:
        ResourceHandle() : m_id(INVALID_RESOURCE_ID), m_type(ResourceType::Unknown) {}
        
        explicit ResourceHandle(ResourceID id, ResourceType type) 
            : m_id(id), m_type(type) {}
        
        /// Check if handle is valid
        bool IsValid() const { return m_id != INVALID_RESOURCE_ID; }
        
        /// Get the resource ID
        ResourceID GetID() const { return m_id; }
        
        /// Get the resource type
        ResourceType GetType() const { return m_type; }
        
        /// Equality comparison
        bool operator==(const ResourceHandle& other) const {
            return m_id == other.m_id && m_type == other.m_type;
        }
        
        bool operator!=(const ResourceHandle& other) const {
            return !(*this == other);
        }
        
        /// Comparison for containers
        bool operator<(const ResourceHandle& other) const {
            if (m_type != other.m_type) {
                return m_type < other.m_type;
            }
            return m_id < other.m_id;
        }
        
        /// Hash support for unordered containers
        struct Hash {
            std::size_t operator()(const ResourceHandle& handle) const {
                return std::hash<uint64_t>{}(handle.m_id) ^ 
                       (std::hash<uint16_t>{}(static_cast<uint16_t>(handle.m_type)) << 1);
            }
        };

    private:
        ResourceID m_id;
        ResourceType m_type;
    };

    /// Resource registry manages the mapping between handles and actual resources
    class ResourceRegistry {
    public:
        static ResourceRegistry& GetInstance();
        
        // Delete copy and move operations for singleton
        ResourceRegistry(const ResourceRegistry&) = delete;
        ResourceRegistry& operator=(const ResourceRegistry&) = delete;
        ResourceRegistry(ResourceRegistry&&) = delete;
        ResourceRegistry& operator=(ResourceRegistry&&) = delete;
        
        /// Register a new resource and get a handle
        template<typename T>
        ResourceHandle<T> Register(std::shared_ptr<T> resource, ResourceType type, const std::string& name = "");
        
        /// Get resource by handle
        template<typename T>
        std::shared_ptr<T> Get(const ResourceHandle<T>& handle);
        
        /// Update existing resource (for hot reloading)
        template<typename T>
        bool Update(const ResourceHandle<T>& handle, std::shared_ptr<T> newResource);
        
        /// Remove resource from registry
        template<typename T>
        bool Remove(const ResourceHandle<T>& handle);
        
        /// Check if handle points to valid resource
        template<typename T>
        bool IsValid(const ResourceHandle<T>& handle);
        
        /// Get resource name by handle
        template<typename T>
        std::string GetName(const ResourceHandle<T>& handle);
        
        /// Get all handles of a specific type
        template<typename T>
        std::vector<ResourceHandle<T>> GetAllHandles(ResourceType type);
        
        /// Clear all resources (shutdown)
        void Clear();
        
        /// Get resource count by type
        size_t GetResourceCount(ResourceType type);
        
        /// Get total resource count
        size_t GetTotalResourceCount();

    private:
        ResourceRegistry() = default;
        ~ResourceRegistry() = default;
        
        /// Generate unique resource ID
        ResourceID GenerateID();
        
        /// Resource entry in the registry
        struct ResourceEntry {
            std::shared_ptr<void> resource;
            ResourceType type;
            std::string name;
            std::chrono::steady_clock::time_point creationTime;
            std::chrono::steady_clock::time_point lastAccessTime;
        };
        
        std::unordered_map<ResourceID, ResourceEntry> m_resources;
        std::atomic<ResourceID> m_nextID{1};
        std::mutex m_registryMutex;
    };

    /// Template implementations
    template<typename T>
    ResourceHandle<T> ResourceRegistry::Register(std::shared_ptr<T> resource, ResourceType type, const std::string& name) {
        if (!resource) {
            return ResourceHandle<T>{};
        }
        
        std::lock_guard<std::mutex> lock(m_registryMutex);
        
        ResourceID id = GenerateID();
        ResourceEntry entry;
        entry.resource = std::static_pointer_cast<void>(resource);
        entry.type = type;
        entry.name = name;
        entry.creationTime = std::chrono::steady_clock::now();
        entry.lastAccessTime = entry.creationTime;
        
        m_resources[id] = std::move(entry);
        
        return ResourceHandle<T>(id, type);
    }
    
    template<typename T>
    std::shared_ptr<T> ResourceRegistry::Get(const ResourceHandle<T>& handle) {
        if (!handle.IsValid()) {
            return nullptr;
        }
        
        std::lock_guard<std::mutex> lock(m_registryMutex);
        
        auto it = m_resources.find(handle.GetID());
        if (it == m_resources.end()) {
            return nullptr;
        }
        
        auto& entry = it->second;
        if (entry.type != handle.GetType()) {
            return nullptr;
        }
        
        entry.lastAccessTime = std::chrono::steady_clock::now();
        return std::static_pointer_cast<T>(entry.resource);
    }
    
    template<typename T>
    bool ResourceRegistry::Update(const ResourceHandle<T>& handle, std::shared_ptr<T> newResource) {
        if (!handle.IsValid() || !newResource) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(m_registryMutex);
        
        auto it = m_resources.find(handle.GetID());
        if (it == m_resources.end()) {
            return false;
        }
        
        auto& entry = it->second;
        if (entry.type != handle.GetType()) {
            return false;
        }
        
        entry.resource = std::static_pointer_cast<void>(newResource);
        entry.lastAccessTime = std::chrono::steady_clock::now();
        
        return true;
    }
    
    template<typename T>
    bool ResourceRegistry::Remove(const ResourceHandle<T>& handle) {
        if (!handle.IsValid()) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(m_registryMutex);
        
        auto it = m_resources.find(handle.GetID());
        if (it == m_resources.end()) {
            return false;
        }
        
        if (it->second.type != handle.GetType()) {
            return false;
        }
        
        m_resources.erase(it);
        return true;
    }
    
    template<typename T>
    bool ResourceRegistry::IsValid(const ResourceHandle<T>& handle) {
        if (!handle.IsValid()) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(m_registryMutex);
        
        auto it = m_resources.find(handle.GetID());
        return it != m_resources.end() && it->second.type == handle.GetType();
    }
    
    template<typename T>
    std::string ResourceRegistry::GetName(const ResourceHandle<T>& handle) {
        if (!handle.IsValid()) {
            return "";
        }
        
        std::lock_guard<std::mutex> lock(m_registryMutex);
        
        auto it = m_resources.find(handle.GetID());
        if (it == m_resources.end() || it->second.type != handle.GetType()) {
            return "";
        }
        
        return it->second.name;
    }
    
    template<typename T>
    std::vector<ResourceHandle<T>> ResourceRegistry::GetAllHandles(ResourceType type) {
        std::vector<ResourceHandle<T>> handles;
        
        std::lock_guard<std::mutex> lock(m_registryMutex);
        
        for (const auto& [id, entry] : m_resources) {
            if (entry.type == type) {
                handles.emplace_back(id, type);
            }
        }
        
        return handles;
    }

    /// Specific handle types for common resources
    class Texture;
    class Shader;
    class Mesh;
    
    using TextureHandle = ResourceHandle<Texture>;
    using ShaderHandle = ResourceHandle<Shader>;
    using MeshHandle = ResourceHandle<Mesh>;

} // namespace SilicaEngine