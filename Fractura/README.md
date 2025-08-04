# 🌸 Fractura: Shatter Garden

> *A visually rich, procedurally animated cube garden powered by SilicaEngine*

![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.6-green.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)

**Fractura: Shatter Garden** is a mesmerizing interactive demo that transforms thousands of colorful cubes into a living, breathing digital garden. Experience procedural beauty through mathematical art as you fly through a surreal world where geometry dances in perfect harmony.

---

## ✨ Features

### 🎨 **Visual Experience**
- **2,700 Animated Cubes**: Massive 30×30×3 grid garden
- **HSV Color Generation**: Vibrant rainbow spectrum across the garden
- **Enhanced Shaders**: Atmospheric lighting, glow effects, and rim lighting
- **Procedural Vertex Animation**: Subtle micro-movements for extra visual detail
- **Dynamic Background**: Atmospheric colors that shift over time
- **Distance Fog**: Adds depth and atmosphere to the scene

### 🎮 **Interactive Controls**

| Input | Action |
|-------|--------|
| **WASD** | Move camera through the garden |
| **Mouse** | Free-look camera control |
| **F** | Toggle wireframe mode |
| **G** | Regenerate garden with new colors |
| **T** | Toggle time freeze |
| **1** | Wave animation mode |
| **2** | Pulse animation mode |
| **3** | Orbit animation mode |
| **4** | Chaos animation mode |
| **F2** | Toggle performance statistics |
| **ESC** | Exit application |

### 🌊 **Animation Modes**

#### 1. **Wave Mode** (Default)
- Sine wave ripples flow across the garden
- Creates ocean-like movement patterns
- Smooth, hypnotic motion

#### 2. **Pulse Mode**
- Radial pulsing from the center outward
- Cubes expand and contract rhythmically
- Creates heartbeat-like effects

#### 3. **Orbit Mode**
- Cubes orbit around invisible attraction points
- Complex circular and helical movements
- Simulates planetary motion

#### 4. **Chaos Mode**
- Procedural noise-based movement
- Organic, unpredictable patterns
- Uses GLM simplex noise for realism

---

## 🚀 Performance

### **Rendering Statistics**
- **FPS**: 60+ (VSync enabled)
- **Draw Calls**: Only 3 (ultra-efficient batch rendering)
- **Vertices**: 97,200
- **Triangles**: 32,400
- **Memory Usage**: Optimized instanced rendering

### **Technical Achievements**
- **Batch Rendering**: All cubes rendered in minimal draw calls
- **Real-time Animation**: Smooth 60fps with complex calculations
- **Efficient Memory**: Smart cube storage and updating
- **Scalable Architecture**: Easy to modify cube count and effects

---

## 🔧 Requirements

### **System Requirements**
- **OS**: Windows 10/11, Linux, or macOS
- **GPU**: OpenGL 4.6 compatible graphics card
- **RAM**: 4GB minimum, 8GB recommended
- **CPU**: Dual-core processor or better

### **Dependencies**
- **OpenGL**: Version 4.6 or higher
- **GLFW**: Window management (automatically fetched)
- **GLM**: Math library version 1.0.1 (automatically fetched)
- **spdlog**: Logging library (automatically fetched)

---

## 🏗️ Building

### **Quick Start**

```bash
# Clone the repository
git clone <repository-url>
cd Caelis/Fractura

# Build (Windows)
.\scripts\windows\build.bat

# Build (Linux/macOS)
./scripts/linux/build.sh
# or
./scripts/macos/build.sh

# Run
.\scripts\windows\run-fractura.bat
# or
./scripts/linux/run-fractura.sh
```

### **Manual Build**

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build with multiple cores
cmake --build . --config Release --parallel

# Run
./bin/Release/Fractura
```

---

## 🎯 Design Philosophy

### **Concept**
Fractura: Shatter Garden embodies the concept of **"Minecraft creative mode meets Monument Valley with cubes and bloom"** - a pure visual experience focused on procedural beauty and mathematical art.

### **Goals**
- **Meditative Experience**: Create a calming, mesmerizing environment
- **Mathematical Beauty**: Showcase algorithmic art through geometry
- **Performance Excellence**: Demonstrate efficient rendering techniques
- **Interactive Exploration**: Allow users to discover different visual modes

### **Visual Inspiration**
- **Monument Valley**: Clean geometric aesthetics
- **Minecraft**: Blocky, voxel-based world
- **Bloom Effects**: Ethereal glow and atmospheric lighting
- **Procedural Art**: Algorithm-generated beauty

---

## 🛠️ Technical Details

### **Engine Integration**
- **SilicaEngine**: Custom C++ game engine
- **Batch Renderer**: Optimized instanced rendering system
- **Shader Pipeline**: Custom vertex/fragment shaders
- **Input System**: Responsive camera and interaction controls

### **Rendering Pipeline**
1. **Cube Generation**: Procedural grid creation with unique properties
2. **Animation Update**: Per-frame position/color/scale calculations
3. **Batch Submission**: All cubes submitted to batch renderer
4. **Instanced Rendering**: GPU draws all instances efficiently
5. **Post-Processing**: Atmospheric effects and color enhancement

### **Animation Mathematics**

```cpp
// Wave Mode
float wave = sin(time * speed - distance * frequency + offset);
position.y += wave * amplitude;

// Pulse Mode  
float distance = length(cube.position);
float pulse = sin(time * speed - distance * frequency + offset);
position += normalize(cube.position) * pulse * amplitude;

// Orbit Mode
float angle = time * speed + offset;
position.x += cos(angle) * radius;
position.z += sin(angle) * radius;

// Chaos Mode
position.x += simplex(vec4(position * scale, time)) * amplitude;
```

---

## 📸 Screenshots

*Experience the beauty of procedural animation:*

- **Wave Mode**: Rippling ocean of cubes
- **Pulse Mode**: Rhythmic expansion from center
- **Orbit Mode**: Planetary dance of geometry  
- **Chaos Mode**: Organic, flowing movements
- **Wireframe**: See the underlying structure

---

## 🎨 Customization

### **Modifying the Garden**

```cpp
// In main.cpp - Garden configuration
static constexpr int GARDEN_SIZE_X = 30;  // Width
static constexpr int GARDEN_SIZE_Z = 30;  // Depth  
static constexpr int GARDEN_SIZE_Y = 3;   // Height
static constexpr float CUBE_SPACING = 2.0f; // Distance between cubes
```

### **Adding New Animation Modes**

1. Add new enum value to `AnimationMode`
2. Implement case in `UpdateGardenAnimation()`
3. Add key binding in `OnKeyEvent()`
4. Create unique mathematical pattern

### **Shader Customization**
- **Fragment Shader**: Modify lighting and color effects
- **Vertex Shader**: Add new vertex animations
- **Uniforms**: Pass additional parameters from CPU

---

## 🤝 Contributing

We welcome contributions to enhance the Shatter Garden experience!

### **Areas for Enhancement**
- New animation modes and patterns
- Additional visual effects and shaders
- Performance optimizations
- Platform-specific improvements
- Documentation and examples

### **Getting Started**
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](../LICENSE) file for details.

---

## 🙏 Acknowledgments

- **SilicaEngine**: Custom game engine providing the foundation
- **GLM**: OpenGL Mathematics library for vector operations
- **GLFW**: Multi-platform library for window management
- **Glad**: OpenGL loading library
- **spdlog**: Fast logging library

---

## 📞 Support

For questions, issues, or feature requests:

1. **Issues**: Use GitHub Issues for bug reports
2. **Discussions**: Join community discussions
3. **Documentation**: Check the engine documentation
4. **Examples**: Explore other demo projects

---

*Dive into the Shatter Garden and experience the beauty of mathematical art in motion.* ✨

---

**Built with ❤️ using SilicaEngine**