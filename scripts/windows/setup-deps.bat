@echo off
REM ============================================================================
REM Caelis - Windows Dependencies Setup Script
REM ============================================================================

echo.
echo ============================================================================
echo  Setting up Caelis Dependencies (Windows)
echo ============================================================================

REM Check for CMake
echo Checking for CMake...
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake not found! Please install CMake and add it to PATH.
    echo Download from: https://cmake.org/download/
    pause
    exit /b 1
) else (
    echo CMake found: 
    cmake --version | findstr "cmake version"
)

REM Check for Visual Studio Build Tools
echo.
echo Checking for Visual Studio Build Tools...
where cl >nul 2>&1
if errorlevel 1 (
    echo WARNING: Visual Studio Build Tools not found in PATH.
    echo Please ensure Visual Studio 2019 or 2022 is installed.
    echo You may need to run this from a Visual Studio Developer Command Prompt.
) else (
    echo Visual Studio Build Tools found.
)

REM Check for Git
echo.
echo Checking for Git...
git --version >nul 2>&1
if errorlevel 1 (
    echo WARNING: Git not found! Git is needed for fetching dependencies.
    echo Download from: https://git-scm.com/download/win
) else (
    echo Git found:
    git --version
)

REM Check for Python (needed for GLAD)
echo.
echo Checking for Python...
python --version >nul 2>&1
if errorlevel 1 (
    echo WARNING: Python not found! Python is needed for GLAD generation.
    echo Download from: https://python.org/downloads/
) else (
    echo Python found:
    python --version
)

echo.
echo ============================================================================
echo  Dependency check completed!
echo  If any dependencies are missing, please install them before building.
echo ============================================================================
echo.

pause