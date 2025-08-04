#!/bin/bash
# ============================================================================
# Caelis - Run Fractura (macOS)
# ============================================================================

BUILD_TYPE=${1:-Release}
EXECUTABLE_PATH="build/bin/$BUILD_TYPE/Fractura"

echo ""
echo "============================================================================"
echo "  Running Fractura ($BUILD_TYPE)"
echo "============================================================================"

if [ ! -f "$EXECUTABLE_PATH" ]; then
    echo "ERROR: Fractura executable not found at $EXECUTABLE_PATH"
    echo "Please build the project first using build.sh"
    exit 1
fi

echo "Starting Fractura..."
echo "Working directory: $(pwd)"
echo "Executable: $EXECUTABLE_PATH"
echo ""

cd "build/bin/$BUILD_TYPE"
./Fractura
cd ../../..

echo ""
echo "Fractura has exited."