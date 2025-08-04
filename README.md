# Caelis

A modern C++ multi-project repository featuring the **SilicaEngine** game engine and **Fractura** demonstration game. Built with industry-standard practices, comprehensive documentation, and cross-platform compatibility.

[![Build & Release](https://github.com/Tfc538/SilicaEngine/actions/workflows/release.yml/badge.svg)](https://github.com/Tfc538/SilicaEngine/actions/workflows/release.yml)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-blue)
![OpenGL](https://img.shields.io/badge/OpenGL-4.6-red)
![License](https://img.shields.io/badge/license-MIT-green)

##  Project Overview

### SilicaEngine
A high-performance, modular game engine featuring:
- **Modern OpenGL** (4.6) rendering with GLAD
- **Cross-platform window management** with GLFW
- **High-performance logging** with spdlog
- **Mathematics library** integration with GLM
- **Clean architecture** with separation of concerns
- **Extensible design** for easy customization

### Fractura
A compelling demonstration game showcasing:
- **3D scene rendering** with animated cubes
- **First-person camera controls** (WASD + mouse look)
- **Real-time performance statistics**
- **Dynamic lighting effects**
- **Wireframe mode toggle**
- **Smooth animations and transformations**

##  Quick Start

### Prerequisites

- **C++17** compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake 3.16+**
- **OpenGL 4.6** compatible graphics card and drivers
- **Git** for dependency management

#### Platform-Specific Requirements

**Windows:**
- Visual Studio 2019+ or MinGW-w64
- Windows 10+ recommended

**Linux:**
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake libgl1-mesa-dev libglu1-mesa-dev

# Fedora/Red Hat
sudo dnf install gcc-c++ cmake mesa-libGL-devel mesa-libGLU-devel

# Arch Linux
sudo pacman -S base-devel cmake mesa
```

**macOS:**
```bash
# Using Homebrew
brew install cmake
# Xcode Command Line Tools required
xcode-select --install
```

### Building the Project

1. **Clone the repository:**
```bash
git clone <repository-url>
cd Caelis
```

2. **Create build directory:**
```bash
mkdir build
cd build
```

3. **Configure with CMake:**
```bash
# Release build (recommended)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Debug build (for development)
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

4. **Build the project:**
```bash
cmake --build . --config Release

# Or for parallel build
cmake --build . --config Release --parallel 8
```

5. **Run Fractura:**
```bash
# Windows
./bin/Fractura.exe

# Linux/macOS
./bin/Fractura
```

## 🎮 Controls & Features

### Fractura Game Controls
- **WASD** - Move camera
- **Mouse** - Look around
- **Space** - Move up
- **Left Shift** - Move down
- **R** - Reset camera position
- **F1** - Toggle wireframe mode
- **F2** - Toggle performance statistics
- **ESC** - Exit application

### Engine Features Demonstrated
- ✅ Real-time 3D rendering
- ✅ Dynamic camera system
- ✅ Primitive shape rendering (cubes, lines, grid)
- ✅ Color-based lighting
- ✅ Animation system
- ✅ Input handling
- ✅ Performance monitoring
- ✅ Cross-platform compatibility

## 🏗️ Architecture Overview

### Directory Structure
```
Caelis/
├── SilicaEngine/               # Game engine library
│   ├── include/                # Public headers
│   │   └── SilicaEngine/
│   │       ├── Core/           # Core engine systems
│   │       └── Renderer/       # Rendering systems
│   └── src/                    # Implementation files
│       ├── Core/
│       └── Renderer/
├── Fractura/                   # Demonstration game
│   ├── src/
│   └── assets/                 # Game assets
│       └── shaders/            # GLSL shaders
└── external/                   # Third-party dependencies (auto-managed)
```

### Engine Components

#### Core Systems
- **Application**: Main application loop and lifecycle management
- **Window**: Cross-platform window creation and event handling
- **Logger**: High-performance logging with multiple output targets

#### Renderer Systems
- **Renderer**: High-level rendering interface and state management
- **Shader**: OpenGL shader compilation and uniform management

### Dependency Management

The project uses CMake's `FetchContent` to automatically download and build dependencies:

| Library | Purpose | Version |
|---------|---------|---------|
| **GLFW** | Window management and input | 3.3.9 |
| **GLAD** | OpenGL function loading | 2.0.5 |
| **GLM** | Mathematics library | 1.0.1 |
| **spdlog** | High-performance logging | 1.12.0 |

## 🔧 Configuration Options

### CMake Options
```bash
# Enable debug features
cmake .. -DCMAKE_BUILD_TYPE=Debug -DSILICA_DEBUG=ON

# Disable specific dependencies (use system packages)
cmake .. -DUSE_SYSTEM_GLFW=ON -DUSE_SYSTEM_GLM=ON

# Custom installation directory
cmake .. -DCMAKE_INSTALL_PREFIX=/custom/path
```

### Runtime Configuration
```cpp
// Enable file logging
SilicaEngine::Logger::EnableFileLogging(true, "game.log");

// Set log level
SilicaEngine::Logger::SetLogLevel(spdlog::level::warn);

// Configure window
SilicaEngine::ApplicationConfig config;
config.enableVSync = false;  // Disable VSync for maximum FPS
config.windowResizable = false;  // Fixed window size
```

## 🤝 Contributing

We welcome contributions! Please follow these guidelines:

1. **Fork the repository** and create a feature branch
2. **Update documentation** for API changes
3. **Submit a pull request** with a clear description

### Coding Standards
- Use **modern C++17** features appropriately
- Follow **RAII** principles for resource management
- Prefer **const correctness** and **explicit types**
- Use **meaningful names** for variables and functions
- Document **public APIs** with Doxygen comments

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **GLFW Team** for excellent window management
- **Khronos Group** for OpenGL specifications
- **G-Truc** for the GLM mathematics library
- **Gabi Melman** for the high-performance spdlog library
- **David Herberth** for the GLAD OpenGL loader

## 📞 Support

- **Issues**: Report bugs and request features on GitHub Issues
- **Discussions**: Join community discussions on GitHub Discussions
- **Documentation**: Comprehensive API docs available in source comments

---

**Built with ❤️ for the game development community**
