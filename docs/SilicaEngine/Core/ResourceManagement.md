# Resource Management Modules

The resource management system in SilicaEngine provides a comprehensive solution for managing game assets and resources throughout their lifecycle. It consists of four main components: stable resource handles, debug resource tracking, asset management with reference counting, and dependency tracking for complex asset relationships.

## Key Features

*   **Stable Resource Handles**: Type-safe handles that remain valid across resource reloads and memory changes.
*   **Debug Resource Tracking**: OpenGL resource leak detection in debug builds.
*   **Asset Management**: Centralized asset loading, caching, and reference counting with automatic cleanup.
*   **Dependency Tracking**: Complex asset dependency management with circular dependency detection and topological sorting.
*   **Hot Reloading**: Runtime asset reloading for development workflows.
*   **Thread Safety**: Thread-safe operations where appropriate, with clear documentation of thread safety guarantees.
*   **Memory Management**: Automatic garbage collection and memory usage tracking.

## Core Components

### ResourceHandle System

The `ResourceHandle` system provides stable, type-safe handles to resources that remain valid even when the underlying resource data changes (e.g., during hot reloading).

#### ResourceType Enum

```cpp
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
```

#### ResourceHandle Template

```cpp
template<typename T>
class ResourceHandle {
public:
    ResourceHandle();
    explicit ResourceHandle(ResourceID id, ResourceType type);
    
    bool IsValid() const;
    ResourceID GetID() const;
    ResourceType GetType() const;
    
    // Comparison operators
    bool operator==(const ResourceHandle& other) const;
    bool operator!=(const ResourceHandle& other) const;
    bool operator<(const ResourceHandle& other) const;
    
    // Hash support
    struct Hash;
};
```

#### ResourceRegistry

The `ResourceRegistry` manages the mapping between handles and actual resources:

```cpp
class ResourceRegistry {
public:
    static ResourceRegistry& GetInstance();
    
    // Register and manage resources
    template<typename T>
    ResourceHandle<T> Register(std::shared_ptr<T> resource, ResourceType type, const std::string& name = "");
    
    template<typename T>
    std::shared_ptr<T> Get(const ResourceHandle<T>& handle);
    
    template<typename T>
    bool Update(const ResourceHandle<T>& handle, std::shared_ptr<T> newResource);
    
    template<typename T>
    bool Remove(const ResourceHandle<T>& handle);
    
    template<typename T>
    bool IsValid(const ResourceHandle<T>& handle);
    
    // Utility methods
    template<typename T>
    std::string GetName(const ResourceHandle<T>& handle);
    
    template<typename T>
    std::vector<ResourceHandle<T>> GetAllHandles(ResourceType type);
    
    size_t GetResourceCount(ResourceType type);
    size_t GetTotalResourceCount();
    void Clear();
};
```

### ResourceTracker System

The `ResourceTracker` provides debug-only OpenGL resource leak detection:

```cpp
class ResourceTracker {
public:
    static void TrackResource(uint32_t id, const std::string& type, const std::string& location);
    static void UntrackResource(uint32_t id, const std::string& type);
    static void ReportLeaks();
    static void Clear();
};
```

#### Resource Tracking Macros

```cpp
#ifdef SILICA_DEBUG
    #define SE_TRACK_RESOURCE(id, type) \
        SilicaEngine::ResourceTracker::TrackResource(id, type, std::string(__FILE__) + ":" + std::to_string(__LINE__))
    #define SE_UNTRACK_RESOURCE(id, type) \
        SilicaEngine::ResourceTracker::UntrackResource(id, type)
#else
    #define SE_TRACK_RESOURCE(id, type)
    #define SE_UNTRACK_RESOURCE(id, type)
#endif
```

### AssetManager System

The `AssetManager` provides centralized asset loading, caching, and management:

#### Asset Types and Base Class

```cpp
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

class Asset {
public:
    Asset(const std::string& path, AssetType type);
    virtual ~Asset() = default;
    
    const std::string& GetPath() const;
    AssetType GetType() const;
};
```

#### AssetHandle Template

```cpp
template<typename T>
class AssetHandle {
public:
    AssetHandle();
    AssetHandle(std::shared_ptr<T> asset);
    
    T* operator->() const;
    T& operator*() const;
    explicit operator bool() const;
    
    std::shared_ptr<T> Get() const;
    bool IsValid() const;
    void Reset();
};
```

#### AssetLoadParams

```cpp
struct AssetLoadParams {
    bool forceReload = false;
    bool async = false;
    std::function<void(bool)> callback = nullptr;
};
```

#### AssetManager Interface

```cpp
class AssetManager {
public:
    // Initialization
    static ErrorResult<void> Initialize(const std::string& assetRoot = "assets/");
    static ErrorResult<void> Shutdown();
    static void SetAssetRoot(const std::string& assetRoot);
    static const std::string& GetAssetRoot();
    
    // Generic asset loading
    template<typename T>
    static AssetHandle<T> Load(const std::string& path, const AssetLoadParams& params = AssetLoadParams{});
    
    template<typename T>
    static AssetHandle<T> Get(const std::string& path);
    
    template<typename T>
    static bool IsLoaded(const std::string& path);
    
    template<typename T>
    static void Unload(const std::string& path);
    
    template<typename T>
    static void UnloadAll();
    
    // Specialized asset loading
    static AssetHandle<Texture> LoadTexture(const std::string& path, 
                                           const TextureParams& params = TextureParams{},
                                           const AssetLoadParams& loadParams = AssetLoadParams{});
    
    static AssetHandle<Shader> LoadShader(const std::string& name,
                                         const std::string& vertexPath,
                                         const std::string& fragmentPath,
                                         const AssetLoadParams& params = AssetLoadParams{});
    
    static AssetHandle<Shader> LoadShader(const std::string& path,
                                         const AssetLoadParams& params = AssetLoadParams{});
    
    // Asset management
    static size_t GetLoadedAssetCount();
    template<typename T>
    static size_t GetLoadedAssetCount();
    
    static size_t GetMemoryUsage();
    template<typename T>
    static std::vector<std::string> GetLoadedAssetPaths();
    
    static void CleanupUnusedAssets();
    static void ForceGarbageCollection();
    
    // Hot reloading
    static void SetHotReloadEnabled(bool enabled);
    static void CheckForChanges();
    template<typename T>
    static void RegisterHotReloadCallback(const std::string& path, 
                                        std::function<void(AssetHandle<T>)> callback);
    
    // Default assets
    static AssetHandle<Texture> GetDefaultWhiteTexture();
    static AssetHandle<Texture> GetDefaultBlackTexture();
    static AssetHandle<Texture> GetDefaultCheckerboardTexture();
    static AssetHandle<Shader> GetDefaultShader();
    
    // Asset preprocessing
    template<typename T>
    static void RegisterAssetLoader(std::function<std::shared_ptr<T>(const std::string&)> loader);
};
```

### AssetDependency System

The `AssetDependency` system manages complex relationships between assets:

#### Dependency Types

```cpp
enum class DependencyType {
    Required,    // Asset cannot function without this dependency
    Optional,    // Asset can function but with reduced capability
    Runtime      // Dependency loaded at runtime (e.g., streaming)
};
```

#### AssetDependency Structure

```cpp
struct AssetDependency {
    ResourceID dependentAsset;      // Asset that depends on another
    ResourceID dependencyAsset;     // Asset that is depended upon
    DependencyType type;
    std::string dependencyPath;     // Original path/name for debugging
    
    bool operator==(const AssetDependency& other) const;
};
```

#### AssetDependencyManager

```cpp
class AssetDependencyManager {
public:
    static AssetDependencyManager& GetInstance();
    
    // Dependency management
    void AddDependency(ResourceID dependent, ResourceID dependency, 
                      DependencyType type = DependencyType::Required,
                      const std::string& dependencyPath = "");
    
    void RemoveDependency(ResourceID dependent, ResourceID dependency);
    void RemoveAllDependencies(ResourceID asset);
    
    // Dependency queries
    std::vector<AssetDependency> GetDependencies(ResourceID asset) const;
    std::vector<ResourceID> GetDependents(ResourceID asset) const;
    bool HasDependency(ResourceID dependent, ResourceID dependency) const;
    bool HasCircularDependency(ResourceID asset) const;
    
    // Dependency resolution
    std::vector<ResourceID> GetDependencyChain(ResourceID asset) const;
    std::vector<ResourceID> GetLoadingOrder(const std::vector<ResourceID>& assets) const;
    
    // Validation and cleanup
    std::vector<ResourceID> ValidateDependencies() const;
    std::vector<ResourceID> GetOrphanedAssets() const;
    
    // Callbacks and statistics
    void RegisterDependencyCallback(const DependencyCallback& callback);
    void ClearDependencyCallbacks();
    DependencyStats GetStatistics() const;
    std::string ExportToDOT() const;
    
    void Clear();
};
```

#### DependencyScope

```cpp
class DependencyScope {
public:
    DependencyScope(ResourceID scopeAsset);
    ~DependencyScope();
    
    void AddDependency(ResourceID dependency, DependencyType type = DependencyType::Required,
                      const std::string& path = "");
    void DisableAutoCleanup();
    const std::vector<ResourceID>& GetScopeDependencies() const;
};
```

## Usage Examples

### Resource Handle Usage

```cpp
// Register a texture resource
auto texture = std::make_shared<Texture>();
auto handle = ResourceRegistry::GetInstance().Register(texture, ResourceType::Texture, "player_texture");

// Use the handle to access the resource
auto retrievedTexture = ResourceRegistry::GetInstance().Get(handle);
if (retrievedTexture) {
    // Use the texture
    retrievedTexture->Bind();
}

// Update the resource (e.g., during hot reload)
auto newTexture = std::make_shared<Texture>();
ResourceRegistry::GetInstance().Update(handle, newTexture);
```

### Asset Management Usage

```cpp
// Initialize asset manager
AssetManager::Initialize("assets/");

// Load a texture
auto textureHandle = AssetManager::LoadTexture("textures/player.png");
if (textureHandle) {
    // Use the texture
    textureHandle->Bind();
}

// Load a shader
auto shaderHandle = AssetManager::LoadShader("basic", 
    "shaders/basic.vert", "shaders/basic.frag");
if (shaderHandle) {
    shaderHandle->Use();
}

// Check if asset is loaded
if (AssetManager::IsLoaded<Texture>("textures/player.png")) {
    // Asset is already loaded
}

// Get memory usage
size_t memoryUsage = AssetManager::GetMemoryUsage();
std::cout << "Asset memory usage: " << memoryUsage << " bytes" << std::endl;

// Cleanup unused assets
AssetManager::CleanupUnusedAssets();
```

### Dependency Management Usage

```cpp
auto& depManager = AssetDependencyManager::GetInstance();

// Add dependencies
ResourceID materialID = /* material resource ID */;
ResourceID textureID = /* texture resource ID */;
ResourceID shaderID = /* shader resource ID */;

depManager.AddDependency(materialID, textureID, DependencyType::Required, "diffuse_texture");
depManager.AddDependency(materialID, shaderID, DependencyType::Required, "material_shader");

// Check for circular dependencies
if (depManager.HasCircularDependency(materialID)) {
    SE_ERROR("Circular dependency detected for material");
}

// Get loading order
std::vector<ResourceID> assets = {materialID, textureID, shaderID};
auto loadingOrder = depManager.GetLoadingOrder(assets);
// loadingOrder will contain assets in dependency order (dependencies first)

// Use dependency scope for automatic cleanup
{
    SE_DEPENDENCY_SCOPE(materialID);
    _scope.AddDependency(textureID, DependencyType::Required, "diffuse");
    _scope.AddDependency(shaderID, DependencyType::Required, "shader");
    // Dependencies automatically cleaned up when scope exits
}
```

### Resource Tracking Usage

```cpp
// In OpenGL resource creation
GLuint textureID;
glGenTextures(1, &textureID);
SE_TRACK_RESOURCE(textureID, "Texture");

// In OpenGL resource destruction
SE_UNTRACK_RESOURCE(textureID, "Texture");
glDeleteTextures(1, &textureID);

// Report leaks during shutdown
ResourceTracker::ReportLeaks();
```

### Hot Reloading Usage

```cpp
// Enable hot reloading
AssetManager::SetHotReloadEnabled(true);

// Register callback for texture changes
AssetManager::RegisterHotReloadCallback<Texture>("textures/player.png", 
    [](AssetHandle<Texture> newTexture) {
        if (newTexture) {
            // Update game objects using this texture
            playerSprite.SetTexture(newTexture);
        }
    });

// Check for changes (call periodically)
AssetManager::CheckForChanges();
```

### Custom Asset Loader

```cpp
// Register custom loader for custom asset type
AssetManager::RegisterAssetLoader<CustomAsset>([](const std::string& path) {
    // Custom loading logic
    auto asset = std::make_shared<CustomAsset>();
    if (asset->LoadFromFile(path)) {
        return asset;
    }
    return std::shared_ptr<CustomAsset>();
});

// Use the custom loader
auto customAsset = AssetManager::Load<CustomAsset>("custom/data.asset");
```

## Implementation Details

### Thread Safety

*   **ResourceRegistry**: Thread-safe with mutex protection for all operations.
*   **AssetManager**: **NOT thread-safe** for concurrent access. All public methods must be called from the same thread or external synchronization must be provided.
*   **AssetDependencyManager**: Thread-safe with shared mutex for read/write operations.
*   **ResourceTracker**: Thread-safe with mutex protection.

### Memory Management

The asset management system uses reference counting with `std::shared_ptr` for automatic cleanup. The `CleanupUnusedAssets()` method removes assets with reference count 1 (only held by the asset manager).

### Dependency Resolution

The dependency system uses Tarjan's algorithm for strongly connected component detection to identify circular dependencies, and topological sorting to determine loading order.

### Resource Tracking

Resource tracking is only active in debug builds (`SILICA_DEBUG` defined). In release builds, the tracking macros expand to nothing, providing zero overhead.

### Hot Reloading

Hot reloading monitors file modification times and automatically reloads changed assets. Callbacks are triggered when assets are reloaded, allowing game objects to update their references.

## Performance Considerations

*   **Resource Handles**: Provide O(1) access to resources with minimal overhead.
*   **Asset Caching**: Prevents redundant loading of the same assets.
*   **Dependency Resolution**: Uses efficient graph algorithms for dependency analysis.
*   **Memory Tracking**: Minimal overhead for memory usage statistics.
*   **Resource Tracking**: Zero overhead in release builds.

The resource management system is designed to be efficient for both development and production use, with debug features that can be completely disabled in release builds. 