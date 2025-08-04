@echo off
REM ============================================================================
REM Caelis - Run Fractura (Windows)
REM ============================================================================

setlocal enabledelayedexpansion

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

set EXECUTABLE_PATH=build\bin\%BUILD_TYPE%\Fractura.exe

echo.
echo ============================================================================
echo  Running Fractura (%BUILD_TYPE%)
echo ============================================================================

if not exist "%EXECUTABLE_PATH%" (
    echo ERROR: Fractura executable not found at %EXECUTABLE_PATH%
    echo Please build the project first using build.bat
    pause
    exit /b 1
)

echo Starting Fractura...
echo Working directory: %cd%
echo Executable: %EXECUTABLE_PATH%
echo.

cd build\bin\%BUILD_TYPE%
Fractura.exe
cd ..\..\..

echo.
echo Fractura has exited.
pause