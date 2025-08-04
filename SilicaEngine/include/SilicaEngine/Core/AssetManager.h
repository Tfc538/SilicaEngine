/**
 * @file AssetManager.h
 * @brief Comprehensive asset management system for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Asset manager with reference counting, resource handles, and automatic loading/unloading.
 */

#pragma once

#include "SilicaEngine/Renderer/Texture.h"
#include "SilicaEngine/Renderer/Shader.h"
#include "ResourceHandle.h"
#include "AssetDependency.h"
#include "SilicaEngine/Core/ErrorCodes.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <typeindex>
#include <functional>
#include <mutex>

namespace SilicaEngine {

    /// Asset types enumeration
    enum class AssetType {
        Texture,
        Shader,
        Mesh,
        Material,
        Audio,
        Font,
        Script,
        Scene
    };

    /// Base class for all assets
    class Asset {
    public:
        Asset(const std::string& path, AssetType type) 
            : m_Path(path), m_Type(type) {}
        virtual ~Asset() = default;
        
        const std::string& GetPath() const { return m_Path; }
        AssetType GetType() const { return m_Type; }
        
    protected:
        std::string m_Path;
        AssetType m_Type;
    };

    /// Asset handle with reference counting
    template<typename T>
    class AssetHandle {
    public:
        AssetHandle() = default;
        AssetHandle(std::shared_ptr<T> asset) : m_Asset(asset) {}
        
        T* operator->() const { return m_Asset.get(); }
        T& operator*() const { return *m_Asset; }
        explicit operator bool() const { return m_Asset != nullptr; }
        
        std::shared_ptr<T> Get() const { return m_Asset; }
        bool IsValid() const { return m_Asset != nullptr; }
        
        void Reset() { m_Asset.reset(); }
        
    private:
        std::shared_ptr<T> m_Asset;
    };

    /// Asset loading parameters
    struct AssetLoadParams {
        bool forceReload = false;
        bool async = false;
        std::function<void(bool)> callback = nullptr;
    };

    /// Asset Manager - Central resource management system
    /// 
    /// Thread Safety: This class is NOT thread-safe for concurrent access.
    /// All public methods must be called from the same thread or external
    /// synchronization must be provided by the client. The internal implementation
    /// uses mutexes to protect shared data structures, but the public interface
    /// is not designed for concurrent access from multiple threads.
    /// 
    /// For multi-threaded asset loading, consider using the async parameter
    /// in AssetLoadParams and implementing proper synchronization at the
    /// application level.
    class AssetManager {
    public:
        /// Initialize the asset manager
        static ErrorResult<void> Initialize(const std::string& assetRoot = "assets/");
        
        /// Shutdown and cleanup all assets
        static ErrorResult<void> Shutdown();
        
        /// Set the root directory for assets
        static void SetAssetRoot(const std::string& assetRoot);
        
        /// Get the asset root directory
        static const std::string& GetAssetRoot();
        
        // === Generic Asset Loading ===
        
        /// Load an asset of specified type
        template<typename T>
        static AssetHandle<T> Load(const std::string& path, const AssetLoadParams& params = AssetLoadParams{});
        
        /// Get an already loaded asset
        template<typename T>
        static AssetHandle<T> Get(const std::string& path);
        
        /// Check if an asset is loaded
        template<typename T>
        static bool IsLoaded(const std::string& path);
        
        /// Unload a specific asset
        template<typename T>
        static void Unload(const std::string& path);
        
        /// Unload all assets of a specific type
        template<typename T>
        static void UnloadAll();
        
        // === Specialized Asset Loading ===
        
        /// Load texture with specific parameters
        static AssetHandle<Texture> LoadTexture(const std::string& path, 
                                               const TextureParams& params = TextureParams{},
                                               const AssetLoadParams& loadParams = AssetLoadParams{});
        
        /// Load shader from vertex and fragment files
        static AssetHandle<Shader> LoadShader(const std::string& name,
                                             const std::string& vertexPath,
                                             const std::string& fragmentPath,
                                             const AssetLoadParams& params = AssetLoadParams{});
        
        /// Load shader from combined file
        static AssetHandle<Shader> LoadShader(const std::string& path,
                                             const AssetLoadParams& params = AssetLoadParams{});
        
        // === Asset Management ===
        
        /// Get number of loaded assets
        static size_t GetLoadedAssetCount();
        
        /// Get number of loaded assets of specific type
        template<typename T>
        static size_t GetLoadedAssetCount();
        
        /// Get memory usage of loaded assets (in bytes)
        static size_t GetMemoryUsage();
        
        /// Get all loaded asset paths of specific type
        template<typename T>
        static std::vector<std::string> GetLoadedAssetPaths();
        
        /// Cleanup unused assets (with ref count 1)
        static void CleanupUnusedAssets();
        
        /// Force garbage collection
        static void ForceGarbageCollection();
        
        // === Hot Reloading ===
        
        /// Enable/disable hot reloading
        static void SetHotReloadEnabled(bool enabled);
        
        /// Check for file changes and reload if necessary
        static void CheckForChanges();
        
        /// Register a hot reload callback for a specific asset
        template<typename T>
        static void RegisterHotReloadCallback(const std::string& path, 
                                            std::function<void(AssetHandle<T>)> callback);
        
        // === Default Assets ===
        
        /// Get default white texture
        static AssetHandle<Texture> GetDefaultWhiteTexture();
        
        /// Get default black texture
        static AssetHandle<Texture> GetDefaultBlackTexture();
        
        /// Get default checkerboard texture
        static AssetHandle<Texture> GetDefaultCheckerboardTexture();
        
        /// Get default shader
        static AssetHandle<Shader> GetDefaultShader();
        
        // === Asset Preprocessing ===
        
        /// Register a custom asset loader
        template<typename T>
        static void RegisterAssetLoader(std::function<std::shared_ptr<T>(const std::string&)> loader);
        
    private:
        class AssetManagerImpl;
        static std::unique_ptr<AssetManagerImpl> s_Instance;
    };

    // === Template Implementations ===
    
    template<typename T>
    AssetHandle<T> AssetManager::Load(const std::string& path, const AssetLoadParams& params) {
        // Implementation will be in the cpp file using explicit template instantiation
        static_assert(std::is_base_of_v<Asset, T> || 
                     std::is_same_v<T, Texture> || 
                     std::is_same_v<T, Shader>, 
                     "T must be an Asset-derived type or supported engine type");
        
        // This will be implemented in the cpp file for supported types
        return AssetHandle<T>{};
    }
    
    template<typename T>
    AssetHandle<T> AssetManager::Get(const std::string& path) {
        // Implementation will be in the cpp file
        return AssetHandle<T>{};
    }
    
    template<typename T>
    bool AssetManager::IsLoaded(const std::string& path) {
        // Implementation will be in the cpp file
        return false;
    }
    
    template<typename T>
    void AssetManager::Unload(const std::string& path) {
        // Implementation will be in the cpp file
    }
    
    template<typename T>
    void AssetManager::UnloadAll() {
        // Implementation will be in the cpp file
    }
    
    template<typename T>
    size_t AssetManager::GetLoadedAssetCount() {
        // Implementation will be in the cpp file
        return 0;
    }
    
    template<typename T>
    std::vector<std::string> AssetManager::GetLoadedAssetPaths() {
        // Implementation will be in the cpp file
        return {};
    }
    
    template<typename T>
    void AssetManager::RegisterHotReloadCallback(const std::string& path, 
                                                std::function<void(AssetHandle<T>)> callback) {
        // Implementation will be in the cpp file
    }
    
    template<typename T>
    void AssetManager::RegisterAssetLoader(std::function<std::shared_ptr<T>(const std::string&)> loader) {
        // Implementation will be in the cpp file
    }

} // namespace SilicaEngine