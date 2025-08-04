@echo off
REM ============================================================================
REM Caelis - Windows Clean Script
REM ============================================================================

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

pause