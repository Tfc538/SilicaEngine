#!/bin/bash
# ============================================================================
# Caelis - Run Fractura (Linux)
# ============================================================================

BUILD_TYPE=${1:-Release}
EXECUTABLE_PATH="build/bin/Fractura"

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

cd build/bin
./Fractura
cd ../..

echo ""
echo "Fractura has exited."