#!/bin/bash
# ============================================================================
# Caelis - Linux Clean Script
# ============================================================================

echo ""
echo "============================================================================"
echo "  Cleaning Caelis Build Files"
echo "============================================================================"

if [ -d "build" ]; then
    echo "Removing build directory..."
    rm -rf build
    echo "Build directory removed."
else
    echo "No build directory found."
fi

if [ -d "install" ]; then
    echo "Removing install directory..."
    rm -rf install
    echo "Install directory removed."
else
    echo "No install directory found."
fi

echo ""
echo "============================================================================"
echo "  Clean completed!"
echo "============================================================================"
echo ""