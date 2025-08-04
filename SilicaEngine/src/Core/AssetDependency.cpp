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
#include <stack>

namespace SilicaEngine {

    // Forward declaration for Tarjan's SCC algorithm function
    void FindStronglyConnectedComponents(
        const std::unordered_map<ResourceID, std::unordered_set<ResourceID>>& graph,
        std::unordered_map<ResourceID, int>& indexMap,
        std::unordered_map<ResourceID, int>& lowlinkMap,
        std::stack<ResourceID>& stack,
        std::unordered_set<ResourceID>& onStack,
        int& index,
        std::vector<std::vector<ResourceID>>& sccs,
        ResourceID v);

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
            
            // Simulate the insertion for the check
            auto tempDependentToSubjects = m_dependentToSubjects;
            tempDependentToSubjects[dependent].insert(dependency);
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
        return HasCircularDependencyRecursive(asset, visited, recursionStack, m_dependentToSubjects);
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
        DependencyStats stats{};
        std::unordered_set<ResourceID> allAssets;
        for (const auto& dep : m_dependencies) {
            allAssets.insert(dep.dependentAsset);
            allAssets.insert(dep.dependencyAsset);
        }
        for (ResourceID asset : allAssets) {
            auto it = m_subjectToDependents.find(asset);
            if (it == m_subjectToDependents.end() || it->second.empty()) {
                stats.orphanedAssets++;
            }
        }
        // Tarjan's SCC for all cycles
        std::unordered_map<ResourceID, int> indexMap, lowlinkMap;
        std::stack<ResourceID> stack;
        std::unordered_set<ResourceID> onStack;
        int index = 0;
        std::vector<std::vector<ResourceID>> sccs;
        for (ResourceID asset : allAssets) {
            if (indexMap.find(asset) == indexMap.end()) {
                FindStronglyConnectedComponents(m_dependentToSubjects, indexMap, lowlinkMap, stack, onStack, index, sccs, asset);
            }
        }
        stats.circularDependencies = static_cast<int>(sccs.size());
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
    
    // Tarjan's SCC for cycle detection
    void FindStronglyConnectedComponents(
        const std::unordered_map<ResourceID, std::unordered_set<ResourceID>>& graph,
        std::unordered_map<ResourceID, int>& indexMap,
        std::unordered_map<ResourceID, int>& lowlinkMap,
        std::stack<ResourceID>& stack,
        std::unordered_set<ResourceID>& onStack,
        int& index,
        std::vector<std::vector<ResourceID>>& sccs,
        ResourceID v)
    {
        indexMap[v] = index;
        lowlinkMap[v] = index;
        ++index;
        stack.push(v);
        onStack.insert(v);

        auto it = graph.find(v);
        if (it != graph.end()) {
            for (ResourceID w : it->second) {
                if (indexMap.find(w) == indexMap.end()) {
                    FindStronglyConnectedComponents(graph, indexMap, lowlinkMap, stack, onStack, index, sccs, w);
                    lowlinkMap[v] = std::min(lowlinkMap[v], lowlinkMap[w]);
                } else if (onStack.count(w)) {
                    lowlinkMap[v] = std::min(lowlinkMap[v], indexMap[w]);
                }
            }
        }

        if (lowlinkMap[v] == indexMap[v]) {
            std::vector<ResourceID> scc;
            ResourceID w;
            do {
                w = stack.top(); stack.pop();
                onStack.erase(w);
                scc.push_back(w);
            } while (w != v);
            if (scc.size() > 1) sccs.push_back(scc); // Only cycles
        }
    }

    // Refactored recursive helper
    bool HasCircularDependencyRecursive(
        ResourceID asset,
        std::unordered_set<ResourceID>& visited,
        std::unordered_set<ResourceID>& recursionStack,
        const std::unordered_map<ResourceID, std::unordered_set<ResourceID>>& dependentToSubjects)
    {
        visited.insert(asset);
        recursionStack.insert(asset);
        auto it = dependentToSubjects.find(asset);
        if (it != dependentToSubjects.end()) {
            for (ResourceID dependency : it->second) {
                if (visited.find(dependency) == visited.end()) {
                    if (HasCircularDependencyRecursive(dependency, visited, recursionStack, dependentToSubjects))
                        return true;
                } else if (recursionStack.find(dependency) != recursionStack.end()) {
                    return true;
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