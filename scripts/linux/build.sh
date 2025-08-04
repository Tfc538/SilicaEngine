#!/bin/bash
# ============================================================================
# Caelis - Linux Build Script
# ============================================================================

set -e  # Exit on any error

# Set default build type
BUILD_TYPE=${1:-Release}

echo ""
echo "============================================================================"
echo "  Building Caelis ($BUILD_TYPE)"
echo "============================================================================"

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build the project
echo ""
echo "Building project..."
cmake --build . --config $BUILD_TYPE --parallel $(nproc)

echo ""
echo "============================================================================"
echo "  Build completed successfully!"
echo "  Executables are in: build/bin/"
echo "============================================================================"
echo ""