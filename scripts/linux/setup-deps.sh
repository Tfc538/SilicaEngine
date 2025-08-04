#!/bin/bash
# ============================================================================
# Caelis - Linux Dependencies Setup Script
# ============================================================================

echo ""
echo "============================================================================"
echo "  Setting up Caelis Dependencies (Linux)"
echo "============================================================================"

# Detect package manager
if command -v apt-get &> /dev/null; then
    PACKAGE_MANAGER="apt"
    INSTALL_CMD="sudo apt-get install -y"
    UPDATE_CMD="sudo apt-get update"
elif command -v dnf &> /dev/null; then
    PACKAGE_MANAGER="dnf"
    INSTALL_CMD="sudo dnf install -y"
    UPDATE_CMD="sudo dnf update"
elif command -v yum &> /dev/null; then
    PACKAGE_MANAGER="yum"
    INSTALL_CMD="sudo yum install -y"
    UPDATE_CMD="sudo yum update"
elif command -v pacman &> /dev/null; then
    PACKAGE_MANAGER="pacman"
    INSTALL_CMD="sudo pacman -S --noconfirm"
    UPDATE_CMD="sudo pacman -Sy"
else
    echo "ERROR: No supported package manager found (apt, dnf, yum, pacman)"
    echo "Please install dependencies manually:"
    echo "  - CMake (>= 3.16)"
    echo "  - Build tools (gcc/clang)"
    echo "  - OpenGL development libraries"
    echo "  - X11 development libraries"
    echo "  - Git"
    echo "  - Python3"
    exit 1
fi

echo "Detected package manager: $PACKAGE_MANAGER"

# Check for CMake
echo ""
echo "Checking for CMake..."
if ! command -v cmake &> /dev/null; then
    echo "CMake not found. Installing..."
    $UPDATE_CMD
    if [ "$PACKAGE_MANAGER" = "apt" ]; then
        $INSTALL_CMD cmake build-essential
    elif [ "$PACKAGE_MANAGER" = "dnf" ]; then
        $INSTALL_CMD cmake gcc-c++ make
    elif [ "$PACKAGE_MANAGER" = "yum" ]; then
        $INSTALL_CMD cmake gcc-c++ make
    elif [ "$PACKAGE_MANAGER" = "pacman" ]; then
        $INSTALL_CMD cmake base-devel
    fi
else
    echo "CMake found: $(cmake --version | head -1)"
fi

# Check for OpenGL and X11 libraries
echo ""
echo "Installing OpenGL and X11 development libraries..."
if [ "$PACKAGE_MANAGER" = "apt" ]; then
    $INSTALL_CMD libgl1-mesa-dev libglu1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
elif [ "$PACKAGE_MANAGER" = "dnf" ]; then
    $INSTALL_CMD mesa-libGL-devel mesa-libGLU-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel
elif [ "$PACKAGE_MANAGER" = "yum" ]; then
    $INSTALL_CMD mesa-libGL-devel mesa-libGLU-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel
elif [ "$PACKAGE_MANAGER" = "pacman" ]; then
    $INSTALL_CMD mesa glu libxrandr libxinerama libxcursor libxi
fi

# Check for Git
echo ""
echo "Checking for Git..."
if ! command -v git &> /dev/null; then
    echo "Git not found. Installing..."
    $INSTALL_CMD git
else
    echo "Git found: $(git --version)"
fi

# Check for Python
echo ""
echo "Checking for Python..."
if ! command -v python3 &> /dev/null; then
    echo "Python3 not found. Installing..."
    $INSTALL_CMD python3
else
    echo "Python3 found: $(python3 --version)"
fi

echo ""
echo "============================================================================"
echo "  Dependency setup completed!"
echo "  You should now be able to build Caelis."
echo "============================================================================"
echo ""