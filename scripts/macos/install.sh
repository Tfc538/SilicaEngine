#!/bin/bash
# ============================================================================
# Caelis - macOS Install Script
# ============================================================================

set -e  # Exit on any error

BUILD_TYPE=${1:-Release}
INSTALL_PREFIX=${2:-$(pwd)/install}

echo ""
echo "============================================================================"
echo "  Installing Caelis ($BUILD_TYPE)"
echo "  Install prefix: $INSTALL_PREFIX"
echo "============================================================================"

if [ ! -d "build" ]; then
    echo "ERROR: Build directory not found. Please build the project first."
    exit 1
fi

cd build

echo "Installing to $INSTALL_PREFIX..."
cmake --install . --config $BUILD_TYPE --prefix "$INSTALL_PREFIX"

cd ..

echo ""
echo "============================================================================"
echo "  Installation completed successfully!"
echo "  Files installed to: $INSTALL_PREFIX"
echo "============================================================================"
echo ""