@echo off
REM ============================================================================
REM Caelis - Windows Build Script
REM ============================================================================

setlocal enabledelayedexpansion

REM Set default build type
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

echo.
echo ============================================================================
echo  Building Caelis (%BUILD_TYPE%)
echo ============================================================================

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring CMake...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
echo.
echo Building project...
cmake --build . --config %BUILD_TYPE% --parallel
if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo ============================================================================
echo  Build completed successfully!
echo  Executables are in: build\bin\%BUILD_TYPE%\
echo ============================================================================
echo.

pause