@echo off
REM ============================================================================
REM Caelis - Windows Build Script
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
if %IS_CI%==1 (
    cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -G "Visual Studio 17 2022" -A x64 -DCMAKE_VERBOSE_MAKEFILE=ON
) else (
    cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -G "Visual Studio 17 2022" -A x64
)
if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed!
    if %IS_CI%==0 pause
    exit /b 1
)

REM Get number of CPU cores for parallel build
for /f "tokens=*" %%i in ('wmic cpu get NumberOfCores /value ^| find "="') do set %%i
set /a CORES=%NumberOfCores%

REM Build the project
echo.
echo Building project with %CORES% cores...
if %IS_CI%==1 (
    cmake --build . --config %BUILD_TYPE% --parallel %CORES% --verbose
) else (
    cmake --build . --config %BUILD_TYPE% --parallel %CORES%
)
if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    if %IS_CI%==0 pause
    exit /b 1
)

echo.
echo ============================================================================
echo  Build completed successfully!
echo  Executables are in: build\bin\%BUILD_TYPE%\
echo ============================================================================
echo.
