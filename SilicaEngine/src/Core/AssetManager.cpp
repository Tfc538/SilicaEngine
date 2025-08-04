/**
 * @file AssetManager.cpp
 * @brief Implementation of comprehensive asset management system
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Core/AssetManager.h"
#include "SilicaEngine/Core/Logger.h"
#include "SilicaEngine/Debug/Profiler.h"
#include <filesystem>
#include <fstream>
#include <chrono>

namespace SilicaEngine {

    /// Internal implementation of AssetManager
    class AssetManager::AssetManagerImpl {
    public:
        std::string assetRoot = "assets/";
        bool hotReloadEnabled = false;
        
        // Asset storage
        std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
        std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
        
        // File modification tracking
        std::unordered_map<std::string, std::filesystem::file_time_type> fileModTimes;
        
        // Hot reload callbacks
        std::unordered_map<std::string, std::function<void()>> hotReloadCallbacks;
        
        // Default assets
        std::shared_ptr<Texture> defaultWhiteTexture;
        std::shared_ptr<Texture> defaultBlackTexture;
        std::shared_ptr<Texture> defaultCheckerboardTexture;
        std::shared_ptr<Shader> defaultShader;
        
        // Thread safety
        std::mutex assetMutex;
        
        // File existence cache for performance
        std::unordered_map<std::string, bool> existenceCache;
        std::chrono::steady_clock::time_point lastCacheUpdate;
        std::mutex cacheMutex;
        
        bool Initialize(const std::string& rootPath) {
            this->assetRoot = rootPath;
            
            // Ensure asset directory exists
            if (!std::filesystem::exists(assetRoot)) {
                std::filesystem::create_directories(assetRoot);
                SE_INFO("Created asset directory: {}", assetRoot);
            }
            
            // Initialize default assets
            InitializeDefaultAssets();
            
            SE_INFO("AssetManager initialized with root: {}", assetRoot);
            return true;
        }
        
        void Shutdown() {
            std::lock_guard<std::mutex> lock(assetMutex);
            
            SE_INFO("AssetManager shutting down...");
            
            // Clear all assets
            textures.clear();
            shaders.clear();
            
            // Clear default assets
            defaultWhiteTexture.reset();
            defaultBlackTexture.reset();
            defaultCheckerboardTexture.reset();
            defaultShader.reset();
            
            // Clear callbacks and file times
            hotReloadCallbacks.clear();
            fileModTimes.clear();
            
            SE_INFO("AssetManager shutdown complete");
        }
        
        void InitializeDefaultAssets() {
            // Create default white texture
            defaultWhiteTexture = Texture::CreateWhite();
            
            // Create default black texture
            defaultBlackTexture = Texture::CreateBlack();
            
            // Create default checkerboard texture
            defaultCheckerboardTexture = Texture::CreateCheckerboard(256);
            
            // Create default shader (simple vertex/fragment shader)
            std::string defaultVertexSource = R"(
                #version 330 core
                layout (location = 0) in vec3 aPos;
                layout (location = 1) in vec2 aTexCoord;
                
                uniform mat4 u_ViewProjection = mat4(1.0);
                uniform mat4 u_Model = mat4(1.0);
                
                out vec2 TexCoord;
                
                void main() {
                    gl_Position = u_ViewProjection * u_Model * vec4(aPos, 1.0);
                    TexCoord = aTexCoord;
                }
            )";
            
            std::string defaultFragmentSource = R"(
                #version 330 core
                in vec2 TexCoord;
                out vec4 FragColor;
                
                uniform sampler2D u_Texture;
                uniform vec4 u_Color = vec4(1.0);
                
                void main() {
                    FragColor = texture(u_Texture, TexCoord) * u_Color;
                }
            )";
            
            defaultShader = std::make_shared<Shader>();
            if (!defaultShader->CreateFromString(defaultVertexSource, defaultFragmentSource)) {
                SE_ERROR("Failed to create default shader");
                defaultShader.reset();
            }
            
            SE_INFO("Default assets initialized");
        }
        
        std::string GetFullPath(const std::string& path) {
            if (std::filesystem::path(path).is_absolute()) {
                return path;
            }
            return assetRoot + path;
        }
        
        void UpdateFileModTime(const std::string& path) {
            std::string fullPath = GetFullPath(path);
            if (std::filesystem::exists(fullPath)) {
                fileModTimes[path] = std::filesystem::last_write_time(fullPath);
            }
        }
        
        bool HasFileChanged(const std::string& path) {
            // Cache the full path existence check to avoid repeated filesystem calls
            std::lock_guard<std::mutex> lock(cacheMutex);
            
            auto now = std::chrono::steady_clock::now();
            
            // Update cache every 100ms to balance performance and responsiveness
            if (now - lastCacheUpdate > std::chrono::milliseconds(100)) {
                existenceCache.clear();
                lastCacheUpdate = now;
            }
            
            std::string fullPath = GetFullPath(path);
            
            // Check existence from cache first
            auto existIt = existenceCache.find(fullPath);
            bool exists;
            if (existIt != existenceCache.end()) {
                exists = existIt->second;
            } else {
                exists = std::filesystem::exists(fullPath);
                existenceCache[fullPath] = exists;
            }
            
            if (!exists) return false;
            
            auto it = fileModTimes.find(path);
            if (it == fileModTimes.end()) return true;
            
            auto currentTime = std::filesystem::last_write_time(fullPath);
            return currentTime > it->second;
        }
        
        std::shared_ptr<Texture> LoadTextureInternal(const std::string& path, const TextureParams& params) {
            std::string fullPath = GetFullPath(path);
            
            auto texture = std::make_shared<Texture>();
            if (!texture->LoadFromFile(fullPath, params)) {
                SE_ERROR("Failed to load texture: {}", path);
                return nullptr;
            }
            
            UpdateFileModTime(path);
            return texture;
        }
        
        std::shared_ptr<Shader> LoadShaderInternal(const std::string& path) {
            // For single file shaders, we'll assume they contain vertex and fragment combined
            // This is a simplified approach - in practice you might parse the file
            SE_WARN("Single file shader loading not fully implemented for: {}", path); // TODO: Implement this
            return nullptr;
        }
        
        std::shared_ptr<Shader> LoadShaderInternal(const std::string& name, 
                                                  const std::string& vertexPath, 
                                                  const std::string& fragmentPath) {
            std::string fullVertexPath = GetFullPath(vertexPath);
            std::string fullFragmentPath = GetFullPath(fragmentPath);
            
            auto shader = std::make_shared<Shader>();
            if (!shader->CreateFromFile(fullVertexPath, fullFragmentPath)) {
                SE_ERROR("Failed to load shader: {} (vertex: {}, fragment: {})", name, vertexPath, fragmentPath);
                return nullptr;
            }
            
            UpdateFileModTime(vertexPath);
            UpdateFileModTime(fragmentPath);
            return shader;
        }
    };

    // Static instance
    std::unique_ptr<AssetManager::AssetManagerImpl> AssetManager::s_Instance = nullptr;

    ErrorResult<void> AssetManager::Initialize(const std::string& assetRoot) {
        SE_PROFILE_FUNCTION();
        
        if (s_Instance) {
            SE_WARN("AssetManager already initialized");
            return ErrorResult<void>::Success();
        }
        
        s_Instance = std::make_unique<AssetManagerImpl>();
        if (!s_Instance->Initialize(assetRoot)) {
            return ErrorResult<void>::Error(EngineError::InitializationFailed, "Failed to initialize AssetManager implementation");
        }
        return ErrorResult<void>::Success();
    }

    ErrorResult<void> AssetManager::Shutdown() {
        SE_PROFILE_FUNCTION();
        
        if (s_Instance) {
            s_Instance->Shutdown();
            s_Instance.reset();
        }
        return ErrorResult<void>::Success();
    }

    void AssetManager::SetAssetRoot(const std::string& assetRoot) {
        if (!s_Instance) return;
        s_Instance->assetRoot = assetRoot;
        SE_INFO("Asset root changed to: {}", assetRoot);
    }

    const std::string& AssetManager::GetAssetRoot() {
        static std::string empty;
        if (!s_Instance) return empty;
        return s_Instance->assetRoot;
    }

    AssetHandle<Texture> AssetManager::LoadTexture(const std::string& path, 
                                                  const TextureParams& params,
                                                  const AssetLoadParams& loadParams) {
        if (!s_Instance) return AssetHandle<Texture>{};
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        
        // Check if already loaded and not forcing reload
        if (!loadParams.forceReload) {
            auto it = s_Instance->textures.find(path);
            if (it != s_Instance->textures.end()) {
                return AssetHandle<Texture>{it->second};
            }
        }
        
        // Load the texture
        auto texture = s_Instance->LoadTextureInternal(path, params);
        if (!texture) {
            return AssetHandle<Texture>{};
        }
        
        // Store in cache
        s_Instance->textures[path] = texture;
        
        SE_INFO("Texture loaded: {}", path);
        return AssetHandle<Texture>{texture};
    }

    AssetHandle<Shader> AssetManager::LoadShader(const std::string& name,
                                                const std::string& vertexPath,
                                                const std::string& fragmentPath,
                                                const AssetLoadParams& params) {
        if (!s_Instance) return AssetHandle<Shader>{};
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        
        // Check if already loaded and not forcing reload
        if (!params.forceReload) {
            auto it = s_Instance->shaders.find(name);
            if (it != s_Instance->shaders.end()) {
                return AssetHandle<Shader>{it->second};
            }
        }
        
        // Load the shader
        auto shader = s_Instance->LoadShaderInternal(name, vertexPath, fragmentPath);
        if (!shader) {
            return AssetHandle<Shader>{};
        }
        
        // Store in cache
        s_Instance->shaders[name] = shader;
        
        SE_INFO("Shader loaded: {} (vertex: {}, fragment: {})", name, vertexPath, fragmentPath);
        return AssetHandle<Shader>{shader};
    }

    AssetHandle<Shader> AssetManager::LoadShader(const std::string& path,
                                                const AssetLoadParams& params) {
        if (!s_Instance) return AssetHandle<Shader>{};
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        
        // Check if already loaded and not forcing reload
        if (!params.forceReload) {
            auto it = s_Instance->shaders.find(path);
            if (it != s_Instance->shaders.end()) {
                return AssetHandle<Shader>{it->second};
            }
        }
        
        // Load the shader
        auto shader = s_Instance->LoadShaderInternal(path);
        if (!shader) {
            return AssetHandle<Shader>{};
        }
        
        // Store in cache
        s_Instance->shaders[path] = shader;
        
        SE_INFO("Shader loaded: {}", path);
        return AssetHandle<Shader>{shader};
    }

    size_t AssetManager::GetLoadedAssetCount() {
        if (!s_Instance) return 0;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        return s_Instance->textures.size() + s_Instance->shaders.size();
    }

    size_t AssetManager::GetMemoryUsage() {
        if (!s_Instance) return 0;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        
        size_t totalMemory = 0;
        
        // Estimate texture memory usage
        for (const auto& [path, texture] : s_Instance->textures) {
            if (texture && texture->IsValid()) {
                // Rough estimation: width * height * channels * sizeof(byte)
                totalMemory += texture->GetWidth() * texture->GetHeight() * texture->GetChannels();
            }
        }
        
        // Shader memory is minimal compared to textures
        totalMemory += s_Instance->shaders.size() * 1024; // Rough estimate
        
        return totalMemory;
    }

    void AssetManager::CleanupUnusedAssets() {
        if (!s_Instance) return;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        
        size_t removedCount = 0;
        
        // Cleanup textures with ref count 1 (only held by asset manager)
        for (auto it = s_Instance->textures.begin(); it != s_Instance->textures.end();) {
            if (it->second.use_count() == 1) {
                SE_INFO("Cleaning up unused texture: {}", it->first);
                it = s_Instance->textures.erase(it);
                removedCount++;
            } else {
                ++it;
            }
        }
        
        // Cleanup shaders with ref count 1
        for (auto it = s_Instance->shaders.begin(); it != s_Instance->shaders.end();) {
            if (it->second.use_count() == 1) {
                SE_INFO("Cleaning up unused shader: {}", it->first);
                it = s_Instance->shaders.erase(it);
                removedCount++;
            } else {
                ++it;
            }
        }
        
        if (removedCount > 0) {
            SE_INFO("Cleaned up {} unused assets", removedCount);
        }
    }

    void AssetManager::ForceGarbageCollection() {
        CleanupUnusedAssets();
    }

    void AssetManager::SetHotReloadEnabled(bool enabled) {
        if (!s_Instance) return;
        s_Instance->hotReloadEnabled = enabled;
        SE_INFO("Hot reloading {}", enabled ? "enabled" : "disabled");
    }

    void AssetManager::CheckForChanges() {
        if (!s_Instance || !s_Instance->hotReloadEnabled) return;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        
        // Check textures for changes
        for (auto& [path, texture] : s_Instance->textures) {
            if (s_Instance->HasFileChanged(path)) {
                SE_INFO("Reloading changed texture: {}", path);
                TextureParams params = texture ? texture->GetParams() : TextureParams{};
                auto newTexture = s_Instance->LoadTextureInternal(path, params);
                if (newTexture) {
                    s_Instance->textures[path] = newTexture;
                }
            }
        }
        
        // Check shaders for changes
        for (auto& [path, shader] : s_Instance->shaders) {
            if (s_Instance->HasFileChanged(path)) {
                SE_INFO("Reloading changed shader: {}", path);
                auto newShader = s_Instance->LoadShaderInternal(path);
                if (newShader) {
                    s_Instance->shaders[path] = newShader;
                }
            }
        }
    }

    AssetHandle<Texture> AssetManager::GetDefaultWhiteTexture() {
        if (!s_Instance || !s_Instance->defaultWhiteTexture) return AssetHandle<Texture>{};
        return AssetHandle<Texture>{s_Instance->defaultWhiteTexture};
    }

    AssetHandle<Texture> AssetManager::GetDefaultBlackTexture() {
        if (!s_Instance || !s_Instance->defaultBlackTexture) return AssetHandle<Texture>{};
        return AssetHandle<Texture>{s_Instance->defaultBlackTexture};
    }

    AssetHandle<Texture> AssetManager::GetDefaultCheckerboardTexture() {
        if (!s_Instance || !s_Instance->defaultCheckerboardTexture) return AssetHandle<Texture>{};
        return AssetHandle<Texture>{s_Instance->defaultCheckerboardTexture};
    }

    AssetHandle<Shader> AssetManager::GetDefaultShader() {
        if (!s_Instance || !s_Instance->defaultShader) return AssetHandle<Shader>{};
        return AssetHandle<Shader>{s_Instance->defaultShader};
    }

    // Template explicit instantiations for supported types
    template<>
    AssetHandle<Texture> AssetManager::Load<Texture>(const std::string& path, const AssetLoadParams& params) {
        return LoadTexture(path, TextureParams{}, params);
    }

    template<>
    AssetHandle<Shader> AssetManager::Load<Shader>(const std::string& path, const AssetLoadParams& params) {
        return LoadShader(path, params);
    }

    template<>
    AssetHandle<Texture> AssetManager::Get<Texture>(const std::string& path) {
        if (!s_Instance) return AssetHandle<Texture>{};
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        auto it = s_Instance->textures.find(path);
        if (it != s_Instance->textures.end()) {
            return AssetHandle<Texture>{it->second};
        }
        return AssetHandle<Texture>{};
    }

    template<>
    AssetHandle<Shader> AssetManager::Get<Shader>(const std::string& path) {
        if (!s_Instance) return AssetHandle<Shader>{};
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        auto it = s_Instance->shaders.find(path);
        if (it != s_Instance->shaders.end()) {
            return AssetHandle<Shader>{it->second};
        }
        return AssetHandle<Shader>{};
    }

    template<>
    bool AssetManager::IsLoaded<Texture>(const std::string& path) {
        if (!s_Instance) return false;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        return s_Instance->textures.find(path) != s_Instance->textures.end();
    }

    template<>
    bool AssetManager::IsLoaded<Shader>(const std::string& path) {
        if (!s_Instance) return false;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        return s_Instance->shaders.find(path) != s_Instance->shaders.end();
    }

    template<>
    void AssetManager::Unload<Texture>(const std::string& path) {
        if (!s_Instance) return;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        auto it = s_Instance->textures.find(path);
        if (it != s_Instance->textures.end()) {
            s_Instance->textures.erase(it);
            SE_INFO("Unloaded texture: {}", path);
        }
    }

    template<>
    void AssetManager::Unload<Shader>(const std::string& path) {
        if (!s_Instance) return;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        auto it = s_Instance->shaders.find(path);
        if (it != s_Instance->shaders.end()) {
            s_Instance->shaders.erase(it);
            SE_INFO("Unloaded shader: {}", path);
        }
    }

    template<>
    void AssetManager::UnloadAll<Texture>() {
        if (!s_Instance) return;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        size_t count = s_Instance->textures.size();
        s_Instance->textures.clear();
        SE_INFO("Unloaded {} textures", count);
    }

    template<>
    void AssetManager::UnloadAll<Shader>() {
        if (!s_Instance) return;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        size_t count = s_Instance->shaders.size();
        s_Instance->shaders.clear();
        SE_INFO("Unloaded {} shaders", count);
    }

    template<>
    size_t AssetManager::GetLoadedAssetCount<Texture>() {
        if (!s_Instance) return 0;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        return s_Instance->textures.size();
    }

    template<>
    size_t AssetManager::GetLoadedAssetCount<Shader>() {
        if (!s_Instance) return 0;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        return s_Instance->shaders.size();
    }

    template<>
    std::vector<std::string> AssetManager::GetLoadedAssetPaths<Texture>() {
        std::vector<std::string> paths;
        if (!s_Instance) return paths;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        for (const auto& [path, texture] : s_Instance->textures) {
            paths.push_back(path);
        }
        return paths;
    }

    template<>
    std::vector<std::string> AssetManager::GetLoadedAssetPaths<Shader>() {
        std::vector<std::string> paths;
        if (!s_Instance) return paths;
        
        std::lock_guard<std::mutex> lock(s_Instance->assetMutex);
        for (const auto& [path, shader] : s_Instance->shaders) {
            paths.push_back(path);
        }
        return paths;
    }

} // namespace SilicaEngine