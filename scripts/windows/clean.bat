@echo off
REM ============================================================================
REM Caelis - Windows Clean Script
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

echo.
echo ============================================================================
echo  Cleaning Caelis Build Files
echo ============================================================================

if exist build (
    echo Removing build directory...
    rmdir /s /q build
    echo Build directory removed.
) else (
    echo No build directory found.
)

if exist install (
    echo Removing install directory...
    rmdir /s /q install
    echo Install directory removed.
) else (
    echo No install directory found.
)

echo.
echo ============================================================================
echo  Clean completed!
echo ============================================================================
echo.

if %IS_CI%==0 pause