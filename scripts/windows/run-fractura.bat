@echo off
REM ============================================================================
REM Caelis - Run Fractura (Windows)
REM Author: Tim Gatzke <post@tim-gatzke.de>
REM ============================================================================

setlocal enabledelayedexpansion

REM Detect CI environment
set IS_CI=0
if "%CI%"=="1" set IS_CI=1
if "%CI%"=="true" set IS_CI=1
if "%GITHUB_ACTIONS%"=="true" set IS_CI=1
if "%APPVEYOR%"=="True" set IS_CI=1
if "%TF_BUILD%"=="True" set IS_CI=1
if not "%JENKINS_URL%"=="" set IS_CI=1

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
    if %IS_CI%==0 pause
    exit /b 1
)

if %IS_CI%==1 (
    echo Skipping Fractura execution in CI environment
    echo Executable exists at: %EXECUTABLE_PATH%
    exit /b 0
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