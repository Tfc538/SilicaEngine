#!/bin/bash
# ============================================================================
# Caelis - macOS Dependencies Setup Script
# ============================================================================

echo ""
echo "============================================================================"
echo "  Setting up Caelis Dependencies (macOS)"
echo "============================================================================"

# Check for Homebrew
if ! command -v brew &> /dev/null; then
    echo "Homebrew not found. Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
else
    echo "Homebrew found: $(brew --version | head -1)"
fi

# Update Homebrew
echo ""
echo "Updating Homebrew..."
brew update

# Check for Xcode Command Line Tools
echo ""
echo "Checking for Xcode Command Line Tools..."
if ! command -v xcode-select &> /dev/null || ! xcode-select -p &> /dev/null; then
    echo "Xcode Command Line Tools not found. Installing..."
    xcode-select --install
    echo "Please follow the installation prompts for Xcode Command Line Tools."
    echo "After installation is complete, run this script again."
    exit 1
else
    echo "Xcode Command Line Tools found: $(xcode-select -p)"
fi

# Check for CMake
echo ""
echo "Checking for CMake..."
if ! command -v cmake &> /dev/null; then
    echo "CMake not found. Installing..."
    brew install cmake
else
    echo "CMake found: $(cmake --version | head -1)"
fi

# Check for Git
echo ""
echo "Checking for Git..."
if ! command -v git &> /dev/null; then
    echo "Git not found. Installing..."
    brew install git
else
    echo "Git found: $(git --version)"
fi

# Check for Python
echo ""
echo "Checking for Python..."
if ! command -v python3 &> /dev/null; then
    echo "Python3 not found. Installing..."
    brew install python
else
    echo "Python3 found: $(python3 --version)"
fi

# Optional: Install additional development tools
echo ""
echo "Installing optional development tools..."
brew install ninja llvm

echo ""
echo "============================================================================"
echo "  Dependency setup completed!"
echo "  You should now be able to build Caelis."
echo "============================================================================"
echo ""