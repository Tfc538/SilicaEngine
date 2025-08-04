/**
 * @file AssetDependency.cpp
 * @brief Implementation of asset dependency tracking system
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Core/AssetDependency.h"
#include "SilicaEngine/Core/Logger.h"
#include <algorithm>
#include <sstream>
#include <shared_mutex>

namespace SilicaEngine {

    AssetDependencyManager& AssetDependencyManager::GetInstance() {
        static AssetDependencyManager instance;
        return instance;
    }
    
    void AssetDependencyManager::AddDependency(ResourceID dependent, ResourceID dependency, 
                                              DependencyType type, const std::string& dependencyPath) {
        AssetDependency dep;
        dep.dependentAsset = dependent;
        dep.dependencyAsset = dependency;
        dep.type = type;
        dep.dependencyPath = dependencyPath;
        
        {
            std::unique_lock<std::shared_mutex> lock(m_dependencyMutex);
            
            // Check for circular dependencies before adding
            if (dependent == dependency) {
                SE_ERROR("Self-dependency detected for asset {}", dependent);
                return;
            }
            
            // Check if adding this dependency would create a circular dependency
            auto tempDeps = m_dependencies;
            tempDeps.insert(dep);
            
            // Create temporary lookup maps for circular dependency check
            auto tempDependentToSubjects = m_dependentToSubjects;
            auto tempSubjectToDependents = m_subjectToDependents;
            
            // Update temporary lookup maps
            tempDependentToSubjects[dependent].insert(dependency);
            tempSubjectToDependents[dependency].insert(dependent);
            
            // Check for circular dependency using temporary data
            std::unordered_set<ResourceID> visited;
            std::unordered_set<ResourceID> recursionStack;
            if (HasCircularDependencyRecursive(dependent, visited, recursionStack, tempDependentToSubjects)) {
                SE_ERROR("Circular dependency detected involving asset {}", dependent);
                return;
            }
            
            // If no circular dependency, add the dependency
            auto result = m_dependencies.insert(dep);
            if (result.second) {  // New dependency added
                UpdateLookupMaps(dep, true);
                SE_INFO("Added dependency: {} -> {}", dependent, dependency);
            }
        }
        
        NotifyCallbacks(dependent, dependency, type);
    }
    
    void AssetDependencyManager::RemoveDependency(ResourceID dependent, ResourceID dependency) {
        AssetDependency dep;
        dep.dependentAsset = dependent;
        dep.dependencyAsset = dependency;
        
        std::unique_lock<std::shared_mutex> lock(m_dependencyMutex);
        
        auto it = m_dependencies.find(dep);
        if (it != m_dependencies.end()) {
            UpdateLookupMaps(*it, false);
            m_dependencies.erase(it);
            SE_INFO("Removed dependency: {} -> {}", dependent, dependency);
        }
    }
    
    void AssetDependencyManager::RemoveAllDependencies(ResourceID asset) {
        std::unique_lock<std::shared_mutex> lock(m_dependencyMutex);
        
        // Remove dependencies where asset is the dependent
        auto it = m_dependentToSubjects.find(asset);
        if (it != m_dependentToSubjects.end()) {
            for (ResourceID dependency : it->second) {
                AssetDependency dep;
                dep.dependentAsset = asset;
                dep.dependencyAsset = dependency;
                m_dependencies.erase(dep);
                
                // Update reverse lookup
                auto reverseIt = m_subjectToDependents.find(dependency);
                if (reverseIt != m_subjectToDependents.end()) {
                    reverseIt->second.erase(asset);
                    if (reverseIt->second.empty()) {
                        m_subjectToDependents.erase(reverseIt);
                    }
                }
            }
            m_dependentToSubjects.erase(it);
        }
        
        // Remove dependencies where asset is the dependency
        auto reverseIt = m_subjectToDependents.find(asset);
        if (reverseIt != m_subjectToDependents.end()) {
            for (ResourceID dependent : reverseIt->second) {
                AssetDependency assetDep;
                assetDep.dependentAsset = dependent;
                assetDep.dependencyAsset = asset;
                m_dependencies.erase(assetDep);
                
                // Update forward lookup
                auto forwardIt = m_dependentToSubjects.find(dependent);
                if (forwardIt != m_dependentToSubjects.end()) {
                    forwardIt->second.erase(asset);
                    if (forwardIt->second.empty()) {
                        m_dependentToSubjects.erase(forwardIt);
                    }
                }
            }
            m_subjectToDependents.erase(reverseIt);
        }
        
        SE_INFO("Removed all dependencies for asset {}", asset);
    }
    
    std::vector<AssetDependency> AssetDependencyManager::GetDependencies(ResourceID asset) const {
        std::shared_lock<std::shared_mutex> lock(m_dependencyMutex);
        
        std::vector<AssetDependency> result;
        
        for (const auto& dep : m_dependencies) {
            if (dep.dependentAsset == asset) {
                result.push_back(dep);
            }
        }
        
        return result;
    }
    
    std::vector<ResourceID> AssetDependencyManager::GetDependents(ResourceID asset) const {
        std::shared_lock<std::shared_mutex> lock(m_dependencyMutex);
        
        auto it = m_subjectToDependents.find(asset);
        if (it != m_subjectToDependents.end()) {
            return std::vector<ResourceID>(it->second.begin(), it->second.end());
        }
        
        return {};
    }
    
    bool AssetDependencyManager::HasDependency(ResourceID dependent, ResourceID dependency) const {
        std::shared_lock<std::shared_mutex> lock(m_dependencyMutex);
        
        // Check direct dependency
        AssetDependency directDep;
        directDep.dependentAsset = dependent;
        directDep.dependencyAsset = dependency;
        
        if (m_dependencies.find(directDep) != m_dependencies.end()) {
            return true;
        }
        
        // Check indirect dependencies (BFS)
        std::unordered_set<ResourceID> visited;
        std::vector<ResourceID> queue;
        
        auto it = m_dependentToSubjects.find(dependent);
        if (it != m_dependentToSubjects.end()) {
            for (ResourceID dep : it->second) {
                queue.push_back(dep);
            }
        }
        
        while (!queue.empty()) {
            ResourceID current = queue.back();
            queue.pop_back();
            
            if (visited.find(current) != visited.end()) {
                continue;
            }
            visited.insert(current);
            
            if (current == dependency) {
                return true;
            }
            
            auto currentIt = m_dependentToSubjects.find(current);
            if (currentIt != m_dependentToSubjects.end()) {
                for (ResourceID nextDep : currentIt->second) {
                    if (visited.find(nextDep) == visited.end()) {
                        queue.push_back(nextDep);
                    }
                }
            }
        }
        
        return false;
    }
    
    bool AssetDependencyManager::HasCircularDependency(ResourceID asset) const {
        std::shared_lock<std::shared_mutex> lock(m_dependencyMutex);
        
        std::unordered_set<ResourceID> visited;
        std::unordered_set<ResourceID> recursionStack;
        
        return HasCircularDependencyRecursive(asset, visited, recursionStack);
    }
    
    std::vector<ResourceID> AssetDependencyManager::GetLoadingOrder(const std::vector<ResourceID>& assets) const {
        std::shared_lock<std::shared_mutex> lock(m_dependencyMutex);
        
        std::unordered_set<ResourceID> visited;
        std::vector<ResourceID> result;
        
        // Perform topological sort
        for (ResourceID asset : assets) {
            if (visited.find(asset) == visited.end()) {
                TopologicalSortUtil(asset, visited, result);
            }
        }
        
        // Reverse to get proper loading order
        std::reverse(result.begin(), result.end());
        return result;
    }
    
    AssetDependencyManager::DependencyStats AssetDependencyManager::GetStatistics() const {
        std::shared_lock<std::shared_mutex> lock(m_dependencyMutex);
        
        DependencyStats stats = {};
        stats.totalDependencies = m_dependencies.size();
        stats.assetsWithDependencies = m_dependentToSubjects.size();
        
        for (const auto& dep : m_dependencies) {
            switch (dep.type) {
                case DependencyType::Required:
                    stats.requiredDependencies++;
                    break;
                case DependencyType::Optional:
                    stats.optionalDependencies++;
                    break;
                case DependencyType::Runtime:
                    stats.runtimeDependencies++;
                    break;
            }
        }
        
        // Count orphaned assets (assets with no dependents)
        std::unordered_set<ResourceID> allAssets;
        for (const auto& [asset, _] : m_dependentToSubjects) {
            allAssets.insert(asset);
        }
        for (const auto& [asset, _] : m_subjectToDependents) {
            allAssets.insert(asset);
        }
        
        for (ResourceID asset : allAssets) {
            auto it = m_subjectToDependents.find(asset);
            if (it == m_subjectToDependents.end() || it->second.empty()) {
                stats.orphanedAssets++;
            }
        }
        
        // Count circular dependencies
        std::unordered_set<ResourceID> checked;
        for (ResourceID asset : allAssets) {
            if (checked.find(asset) == checked.end()) {
                if (HasCircularDependency(asset)) {
                    stats.circularDependencies++;
                }
                checked.insert(asset);
            }
        }
        
        return stats;
    }
    
    void AssetDependencyManager::Clear() {
        std::unique_lock<std::shared_mutex> lock(m_dependencyMutex);
        
        SE_INFO("Clearing asset dependencies ({} total)", m_dependencies.size());
        m_dependencies.clear();
        m_dependentToSubjects.clear();
        m_subjectToDependents.clear();
        m_callbacks.clear();
    }
    
    // === Private Implementation ===
    
    void AssetDependencyManager::UpdateLookupMaps(const AssetDependency& dependency, bool add) {
        if (add) {
            m_dependentToSubjects[dependency.dependentAsset].insert(dependency.dependencyAsset);
            m_subjectToDependents[dependency.dependencyAsset].insert(dependency.dependentAsset);
        } else {
            auto it = m_dependentToSubjects.find(dependency.dependentAsset);
            if (it != m_dependentToSubjects.end()) {
                it->second.erase(dependency.dependencyAsset);
                if (it->second.empty()) {
                    m_dependentToSubjects.erase(it);
                }
            }
            
            auto reverseIt = m_subjectToDependents.find(dependency.dependencyAsset);
            if (reverseIt != m_subjectToDependents.end()) {
                reverseIt->second.erase(dependency.dependentAsset);
                if (reverseIt->second.empty()) {
                    m_subjectToDependents.erase(reverseIt);
                }
            }
        }
    }
    
    void AssetDependencyManager::NotifyCallbacks(ResourceID dependent, ResourceID dependency, DependencyType type) {
        for (const auto& callback : m_callbacks) {
            try {
                callback(dependent, dependency, type);
            } catch (const std::exception& e) {
                SE_ERROR("Exception in dependency callback: {}", e.what());
            }
        }
    }
    
    bool AssetDependencyManager::HasCircularDependencyRecursive(ResourceID asset, 
                                                               std::unordered_set<ResourceID>& visited,
                                                               std::unordered_set<ResourceID>& recursionStack) const {
        visited.insert(asset);
        recursionStack.insert(asset);
        
        auto it = m_dependentToSubjects.find(asset);
        if (it != m_dependentToSubjects.end()) {
            for (ResourceID dependency : it->second) {
                if (visited.find(dependency) == visited.end()) {
                    if (HasCircularDependencyRecursive(dependency, visited, recursionStack)) {
                        return true;
                    }
                } else if (recursionStack.find(dependency) != recursionStack.end()) {
                    return true; // Back edge found - circular dependency
                }
            }
        }
        
        recursionStack.erase(asset);
        return false;
    }
    
    bool AssetDependencyManager::HasCircularDependencyRecursive(ResourceID asset, 
                                                               std::unordered_set<ResourceID>& visited,
                                                               std::unordered_set<ResourceID>& recursionStack,
                                                               const std::unordered_map<ResourceID, std::unordered_set<ResourceID>>& dependentToSubjects) const {
        visited.insert(asset);
        recursionStack.insert(asset);
        
        auto it = dependentToSubjects.find(asset);
        if (it != dependentToSubjects.end()) {
            for (ResourceID dependency : it->second) {
                if (visited.find(dependency) == visited.end()) {
                    if (HasCircularDependencyRecursive(dependency, visited, recursionStack, dependentToSubjects)) {
                        return true;
                    }
                } else if (recursionStack.find(dependency) != recursionStack.end()) {
                    return true; // Back edge found - circular dependency
                }
            }
        }
        
        recursionStack.erase(asset);
        return false;
    }
    
    void AssetDependencyManager::TopologicalSortUtil(ResourceID asset, 
                                                     std::unordered_set<ResourceID>& visited,
                                                     std::vector<ResourceID>& stack) const {
        visited.insert(asset);
        
        auto it = m_dependentToSubjects.find(asset);
        if (it != m_dependentToSubjects.end()) {
            for (ResourceID dependency : it->second) {
                if (visited.find(dependency) == visited.end()) {
                    TopologicalSortUtil(dependency, visited, stack);
                }
            }
        }
        
        stack.push_back(asset);
    }

} // namespace SilicaEngine