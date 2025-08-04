@echo off
REM ============================================================================
REM Caelis - Windows Install Script
REM ============================================================================

setlocal enabledelayedexpansion

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

set INSTALL_PREFIX=%2
if "%INSTALL_PREFIX%"=="" set INSTALL_PREFIX=%cd%\install

echo.
echo ============================================================================
echo  Installing Caelis (%BUILD_TYPE%)
echo  Install prefix: %INSTALL_PREFIX%
echo ============================================================================

if not exist build (
    echo ERROR: Build directory not found. Please build the project first.
    pause
    exit /b 1
)

cd build

echo Installing to %INSTALL_PREFIX%...
cmake --install . --config %BUILD_TYPE% --prefix "%INSTALL_PREFIX%"
if errorlevel 1 (
    echo.
    echo ERROR: Installation failed!
    pause
    exit /b 1
)

cd ..

echo.
echo ============================================================================
echo  Installation completed successfully!
echo  Files installed to: %INSTALL_PREFIX%
echo ============================================================================
echo.

pause