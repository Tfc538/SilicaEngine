# 🧭 Roadmap

### 🚀 v1.1.0 — Input & Asset Loading

**Why:** Unlocks interactive games and structured content.

#### 🆕 Input

* [ ] Key remapping and input configuration system
* [ ] Input context layers (e.g. UI vs gameplay)
* [ ] Gamepad/controller support (via GLFW or SDL)

#### 🆕 Asset Management

* [ ] Texture loading (stb\_image or similar)
* [ ] Basic texture support in shaders
* [ ] Asset manager module with reference counting or handles

#### 🔧 Improvements

* [ ] Configurable default camera controller
* [ ] Screenshot format choice (e.g. PNG vs raw framebuffer)

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
