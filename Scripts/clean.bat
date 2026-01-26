@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Clean Build Script
echo ========================================
echo.

set BUILD_DIR=build

if exist "%BUILD_DIR%" (
    echo Removing build directory...
    rmdir /s /q "%BUILD_DIR%"
    echo Build directory removed successfully.
) else (
    echo Build directory does not exist.
)

echo.
echo Clean completed!
echo.

pause
