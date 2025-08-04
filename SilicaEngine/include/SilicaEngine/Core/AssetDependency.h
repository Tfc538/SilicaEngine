/**
 * @file AssetDependency.h
 * @brief Asset dependency tracking system for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Tracks dependencies between assets (e.g., materials depend on textures).
 */

#pragma once

#include "SilicaEngine/Core/ResourceHandle.h"
#include "SilicaEngine/Core/Logger.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <functional>

namespace SilicaEngine {

    /// Dependency relationship between assets
    enum class DependencyType {
        Required,    // Asset cannot function without this dependency
        Optional,    // Asset can function but with reduced capability
        Runtime      // Dependency loaded at runtime (e.g., streaming)
    };

    /// Single asset dependency
    struct AssetDependency {
        ResourceID dependentAsset;      // Asset that depends on another
        ResourceID dependencyAsset;     // Asset that is depended upon
        DependencyType type;
        std::string dependencyPath;     // Original path/name for debugging
        
        bool operator==(const AssetDependency& other) const {
            return dependentAsset == other.dependentAsset && 
                   dependencyAsset == other.dependencyAsset;
        }
    };

    /// Hash function for AssetDependency
    struct AssetDependencyHash {
        std::size_t operator()(const AssetDependency& dep) const {
            // Use a more robust hash combining approach to reduce collisions
            std::size_t h1 = std::hash<ResourceID>{}(dep.dependentAsset);
            std::size_t h2 = std::hash<ResourceID>{}(dep.dependencyAsset);
            
            // Boost-style hash combine for better distribution
            h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
            return h1;
        }
    };

    /// Callback for dependency events
    using DependencyCallback = std::function<void(ResourceID asset, ResourceID dependency, DependencyType type)>;

    /// Asset dependency tracking and management system
    class AssetDependencyManager {
    public:
        static AssetDependencyManager& GetInstance();
        
        /// Add a dependency relationship
        void AddDependency(ResourceID dependent, ResourceID dependency, 
                          DependencyType type = DependencyType::Required,
                          const std::string& dependencyPath = "");
        
        /// Remove a specific dependency
        void RemoveDependency(ResourceID dependent, ResourceID dependency);
        
        /// Remove all dependencies for an asset
        void RemoveAllDependencies(ResourceID asset);
        
        /// Get all dependencies of an asset
        std::vector<AssetDependency> GetDependencies(ResourceID asset) const;
        
        /// Get all assets that depend on a specific asset
        std::vector<ResourceID> GetDependents(ResourceID asset) const;
        
        /// Check if asset A depends on asset B (direct or indirect)
        bool HasDependency(ResourceID dependent, ResourceID dependency) const;
        
        /// Check for circular dependencies
        bool HasCircularDependency(ResourceID asset) const;
        
        /// Get dependency chain for an asset (topological order)
        std::vector<ResourceID> GetDependencyChain(ResourceID asset) const;
        
        /// Get loading order for a set of assets (resolves dependencies)
        std::vector<ResourceID> GetLoadingOrder(const std::vector<ResourceID>& assets) const;
        
        /// Validate all dependencies (check for missing assets)
        std::vector<ResourceID> ValidateDependencies() const;
        
        /// Get assets that can be safely unloaded (no dependents)
        std::vector<ResourceID> GetOrphanedAssets() const;
        
        /// Register callback for dependency events
        void RegisterDependencyCallback(const DependencyCallback& callback);
        
        /// Clear all dependency callbacks
        void ClearDependencyCallbacks();
        
        /// Get dependency graph statistics
        struct DependencyStats {
            size_t totalDependencies;
            size_t requiredDependencies;
            size_t optionalDependencies;
            size_t runtimeDependencies;
            size_t assetsWithDependencies;
            size_t orphanedAssets;
            size_t circularDependencies;
        };
        DependencyStats GetStatistics() const;
        
        /// Export dependency graph to DOT format for visualization
        std::string ExportToDOT() const;
        
        /// Clear all dependencies (shutdown)
        void Clear();

    private:
        AssetDependencyManager() = default;
        ~AssetDependencyManager() = default;
        
        /// Internal dependency storage
        std::unordered_set<AssetDependency, AssetDependencyHash> m_dependencies;
        
        /// Fast lookup maps
        std::unordered_map<ResourceID, std::unordered_set<ResourceID>> m_dependentToSubjects;  // asset -> what it depends on
        std::unordered_map<ResourceID, std::unordered_set<ResourceID>> m_subjectToDependents; // asset -> what depends on it
        
        /// Callbacks for dependency events
        std::vector<DependencyCallback> m_callbacks;
        
        /// Thread safety
        mutable std::shared_mutex m_dependencyMutex;
        
        /// Helper methods
        void UpdateLookupMaps(const AssetDependency& dependency, bool add);
        void NotifyCallbacks(ResourceID dependent, ResourceID dependency, DependencyType type);
        bool HasCircularDependencyRecursive(ResourceID asset, std::unordered_set<ResourceID>& visited, 
                                           std::unordered_set<ResourceID>& recursionStack) const;
        bool HasCircularDependencyRecursive(ResourceID asset, std::unordered_set<ResourceID>& visited,
                                           std::unordered_set<ResourceID>& recursionStack,
                                           const std::unordered_map<ResourceID, std::unordered_set<ResourceID>>& dependentToSubjects) const;
        void TopologicalSortUtil(ResourceID asset, std::unordered_set<ResourceID>& visited,
                                std::vector<ResourceID>& stack) const;
    };

    /// Dependency scope manager for automatic cleanup
    class DependencyScope {
    public:
        DependencyScope(ResourceID scopeAsset) : m_scopeAsset(scopeAsset) {}
        
        ~DependencyScope() {
            if (m_autoCleanup) {
                try {
                    AssetDependencyManager::GetInstance().RemoveAllDependencies(m_scopeAsset);
                } catch (const std::exception& e) {
                    // Log the error but don't let exceptions propagate from destructor
                    // This ensures the no-throw guarantee for destructors
                    SE_ERROR("Exception in DependencyScope destructor: {}", e.what());
                } catch (...) {
                    // Catch any other exceptions
                    SE_ERROR("Unknown exception in DependencyScope destructor");
                }
            }
        }
        
        /// Add dependency within this scope
        void AddDependency(ResourceID dependency, DependencyType type = DependencyType::Required,
                          const std::string& path = "") {
            AssetDependencyManager::GetInstance().AddDependency(m_scopeAsset, dependency, type, path);
            m_scopeDependencies.push_back(dependency);
        }
        
        /// Disable automatic cleanup
        void DisableAutoCleanup() { m_autoCleanup = false; }
        
        /// Get all dependencies added in this scope
        const std::vector<ResourceID>& GetScopeDependencies() const { return m_scopeDependencies; }

    private:
        ResourceID m_scopeAsset;
        std::vector<ResourceID> m_scopeDependencies;
        bool m_autoCleanup = true;
    };

    /// Convenience macros for dependency management
    #define SE_ASSET_DEPENDS_ON(asset, dependency) \
        SilicaEngine::AssetDependencyManager::GetInstance().AddDependency(asset, dependency)
    
    #define SE_ASSET_DEPENDS_ON_OPTIONAL(asset, dependency) \
        SilicaEngine::AssetDependencyManager::GetInstance().AddDependency(asset, dependency, SilicaEngine::DependencyType::Optional)
    
    #define SE_DEPENDENCY_SCOPE(asset) \
        SilicaEngine::DependencyScope _scope(asset)

} // namespace SilicaEngine