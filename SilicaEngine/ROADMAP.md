# 🧭 Roadmap

### 🚀 v1.1.0 — Input & Asset Loading

**Why:** Unlocks interactive games and structured content.

#### 🆕 Input

* [x] Key remapping and input configuration system
* [x] Input context layers (e.g. UI vs gameplay)
* [x] Gamepad/controller support (via GLFW or SDL)

#### 🆕 Asset Management

* [x] Texture loading (stb\_image or similar)
* [x] Basic texture support in shaders
* [x] Asset manager module with reference counting or handles

#### 🔧 Improvements

* [x] Configurable default camera controller
* [x] Screenshot format choice (e.g. PNG vs raw framebuffer)

---

### 🌐 v1.2.0 — 2D & UI System

**Why:** Adds 2D games and editor capabilities.

#### 🆕 2D Renderer

* [ ] Orthographic projection rendering
* [ ] SpriteBatch with texture atlases
* [ ] Textured quads with UV mapping
* [ ] Anchoring, pivoting, and screen-space transforms

#### 🆕 Font Rendering

* [ ] TrueType/OpenType support (e.g. stb\_truetype or FreeType)
* [ ] Text drawing with alignment and size options
* [ ] Bitmap font support (for lightweight UIs)

#### 🧰 UI Toolkit (basic)

* [ ] Buttons, sliders, toggles
* [ ] Mouse event handling
* [ ] Immediate mode-like UI system (optional)

---

### 📦 v1.3.0 — Asset Pipeline & Scene System

**Why:** Supports real-world game workflows.

#### 🆕 Scene Management

* [ ] Hierarchical scene graph with nodes/components
* [ ] Transform propagation (local/global)
* [ ] Scene serialization (JSON, TOML, etc.)
* [ ] SceneLoader with runtime hot-reload support

#### 🆕 Resource System

* [ ] Unified resource manager for shaders, textures, meshes
* [ ] Optional hot-reloading for shaders
* [ ] Reference-counted smart resource handles

---

### 🧠 v1.4.0 — ECS & Components

**Why:** Enables scalable game logic.

#### 🆕 Entity Component System (ECS)

* [ ] Registry, entity creation/deletion
* [ ] Component definitions (Transform, Camera, MeshRenderer)
* [ ] System registration + update loops
* [ ] Event-based communication (signal/slot or pub/sub)

---

### 🧪 v1.5.0 — Physics & Audio (Feature Tier Breakthrough)

**Why:** This opens doors to full 3D gameplay.

#### 🆕 Physics

* [ ] AABB and OBB support
* [ ] Raycasting, intersection testing
* [ ] Integration with Box2D or Bullet

#### 🆕 Audio

* [ ] Basic sound playback (OpenAL or miniaudio)
* [ ] 2D/3D positional audio
* [ ] Sound asset management


---

### ⚡ v1.6.0 — GPU-Driven Rendering & Compute

**Why:** Massive performance breakthrough: GPU-driven scene management and compute shaders for modern high-performance rendering.

#### 🆕 Compute Shader Infrastructure

* [ ] Compute shader compilation and dispatch system
* [ ] Shader storage buffer objects (SSBO) support
* [ ] Atomic counter buffers for GPU synchronization
* [ ] Work group management and dispatch utilities

#### 🆕 Indirect Rendering

* [ ] Draw indirect command generation
* [ ] Multi-draw indirect rendering
* [ ] GPU-generated draw command buffers
* [ ] Command buffer management and optimization

#### 🆕 GPU Scene Management

* [ ] GPU-side frustum culling with compute shaders
* [ ] GPU LOD selection and management
* [ ] GPU occlusion culling (basic implementation)
* [ ] GPU draw command generation from scene data

#### 🆕 Advanced Memory Management

* [ ] Persistent mapped buffers for GPU-CPU communication
* [ ] GPU-only memory pools and allocation
* [ ] Buffer storage objects for high-performance data transfer
* [ ] Memory barrier and synchronization utilities

#### 🔧 Performance Optimizations

* [ ] GPU-driven animation calculations
* [ ] GPU particle systems
* [ ] GPU instancing with indirect commands
* [ ] Performance monitoring and GPU profiling tools

---

### 🧰 v2.0.0 — Editor & Full Runtime Tools

**Why:** Big milestone: custom content creation, user-facing utilities.

* [ ] ImGui-based Editor overlay
* [ ] Scene inspector, entity hierarchy
* [ ] Material editor
* [ ] Real-time property editing
* [ ] Build & packaging tools

---

### 🛠 Future Modular Milestones

* Scripting (Lua or native C++ hot-reload)
* Networking (Enet, RakNet, or custom TCP/UDP)
* Vulkan backend (experimental)
* Asset importers (gltf, obj, fbx)
* Deferred & PBR rendering
* Full multi-threading support
* Advanced GPU compute (e.g. ray tracing)
* Multi-GPU support and GPU memory pooling
