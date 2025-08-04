#!/bin/bash
# ============================================================================
# Caelis - macOS Build Script
# Author: Tim Gatzke <post@tim-gatzke.de>
# ============================================================================

set -e  # Exit on any error

# Detect CI environment
IS_CI=0
if [[ "$CI" == "1" || "$CI" == "true" ]]; then
    IS_CI=1
elif [[ "$GITHUB_ACTIONS" == "true" ]]; then
    IS_CI=1
elif [[ "$TRAVIS" == "true" ]]; then
    IS_CI=1
elif [[ "$CIRCLECI" == "true" ]]; then
    IS_CI=1
elif [[ "$BUILDKITE" == "true" ]]; then
    IS_CI=1
elif [[ -n "$JENKINS_URL" ]]; then
    IS_CI=1
fi

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
if [[ $IS_CI -eq 1 ]]; then
    # Use Unix Makefiles in CI for better logging and compatibility
    cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE=ON
else
    # Use Xcode generator for better macOS development support
    cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -G "Xcode" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
fi

# Get number of CPU cores for parallel build
CORES=$(sysctl -n hw.ncpu)
echo ""
echo "Building project with \"$CORES\" cores..."
if [[ $IS_CI -eq 1 ]]; then
    cmake --build . --config $BUILD_TYPE --parallel "$CORES" --verbose
else
    cmake --build . --config $BUILD_TYPE --parallel "$CORES"
fi

echo ""
echo "============================================================================"
echo "  Build completed successfully!"
if [[ $IS_CI -eq 1 ]]; then
    echo "  Executables are in: build/bin/"
else
    echo "  Executables are in: build/bin/$BUILD_TYPE/"
fi
echo "============================================================================"
echo ""