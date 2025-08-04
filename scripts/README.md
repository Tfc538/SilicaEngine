# Caelis Build Scripts

This directory contains platform-specific build scripts and utilities for the Caelis game engine project.

## Directory Structure

```
scripts/
├── windows/          # Windows-specific scripts (.bat)
├── linux/            # Linux-specific scripts (.sh)
├── macos/            # macOS-specific scripts (.sh)
├── common/           # Cross-platform Python scripts
└── README.md         # This file
```

## Quick Start

### Windows
```cmd
# Setup dependencies (first time only)
scripts\windows\setup-deps.bat

# Build the project
scripts\windows\build.bat

# Run Fractura game
scripts\windows\run-fractura.bat
```

### Linux/macOS
```bash
# Make scripts executable (first time only)
chmod +x scripts/linux/*.sh
chmod +x scripts/macos/*.sh

# Setup dependencies (first time only)
scripts/linux/setup-deps.sh    # or scripts/macos/setup-deps.sh

# Build the project
scripts/linux/build.sh         # or scripts/macos/build.sh

# Run Fractura game
scripts/linux/run-fractura.sh  # or scripts/macos/run-fractura.sh
```

## Available Scripts

### Build Scripts
- **build.sh/build.bat**: Main build script (defaults to Release)
- **build-debug.sh/build-debug.bat**: Build in Debug mode
- **build-release.sh/build-release.bat**: Build in Release mode

### Utility Scripts
- **clean.sh/clean.bat**: Remove build directories
- **install.sh/install.bat**: Install built binaries and libraries
- **run-fractura.sh/run-fractura.bat**: Run the Fractura example game
- **setup-deps.sh/setup-deps.bat**: Install required dependencies

### Cross-Platform Python Scripts
- **common/format.py**: Format code with clang-format
- **common/analyze.py**: Run static analysis tools (cppcheck, clang-tidy)

## Usage Examples

### Building
```bash
# Build in Release mode (default)
./scripts/linux/build.sh

# Build in Debug mode
./scripts/linux/build.sh Debug

# Or use the convenience scripts
./scripts/linux/build-debug.sh
./scripts/linux/build-release.sh
```

### Code Formatting
```bash
# Format all source code
python3 scripts/common/format.py

# Check if formatting is needed (CI/CD)
python3 scripts/common/format.py --check

# Format specific directory
python3 scripts/common/format.py -d SilicaEngine/
```

### Static Analysis
```bash
# Run all available analysis tools
python3 scripts/common/analyze.py

# Run specific tool
python3 scripts/common/analyze.py --tool cppcheck
python3 scripts/common/analyze.py --tool clang-tidy
```

### Installation
```bash
# Install to default location (./install)
./scripts/linux/install.sh

# Install to custom location
./scripts/linux/install.sh Release /usr/local
```

## Dependencies

### All Platforms
- CMake (>= 3.16)
- Git
- Python 3 (for GLAD generation and common scripts)

### Windows
- Visual Studio 2019 or 2022 (Build Tools)
- Or MinGW-w64 with GCC

### Linux
- GCC or Clang
- OpenGL development libraries
- X11 development libraries

### macOS
- Xcode Command Line Tools
- Homebrew (recommended for package management)

## Build Configuration

The project uses CMake with the following features:
- **Build Types**: Debug, Release
- **C++ Standard**: C++17
- **Dependencies**: OpenGL, GLFW, GLM, spdlog, GLAD
- **Output**: 
  - Libraries in `build/lib/`
  - Executables in `build/bin/`

## Troubleshooting

### Common Issues

1. **CMake not found**
   - Install CMake and add to PATH
   - Run setup-deps script first

2. **OpenGL libraries missing** (Linux)
   - Install mesa development packages
   - Run: `sudo apt install libgl1-mesa-dev`

3. **Build fails with dependency errors**
   - CMake will automatically download dependencies via FetchContent
   - Ensure internet connection is available
   - Check firewall/proxy settings

4. **Python not found** (Windows)
   - Install Python from python.org
   - Add Python to PATH
   - Update Python path in root CMakeLists.txt if needed

### Performance Tips

- Use `-j` flag for parallel builds: `cmake --build . --parallel`
- On Windows, use Visual Studio generator for better performance
- On Linux/macOS, consider using Ninja generator: `cmake .. -G Ninja`

## Contributing

When adding new scripts:
1. Follow the existing naming conventions
2. Add appropriate error handling
3. Include help/usage information
4. Test on the target platform
5. Update this README

## License

These scripts are part of the Caelis project and follow the same license terms.