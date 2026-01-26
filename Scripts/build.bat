@echo off
setlocal enabledelayedexpansion

echo ========================================
echo ChatAndFileTransferSystem Build Script
echo ========================================
echo.

set BUILD_DIR=build
set BUILD_TYPE=Release

if "%1"=="debug" set BUILD_TYPE=Debug
if "%1"=="Debug" set BUILD_TYPE=Debug

echo Build Type: %BUILD_TYPE%
echo.

if not exist "%BUILD_DIR%" (
    echo Creating build directory...
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"

echo Configuring CMake...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if errorlevel 1 (
    echo.
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo Building project...
mingw32-make -j%NUMBER_OF_PROCESSORS%

if errorlevel 1 (
    echo.
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executables are in: %BUILD_DIR%\bin
echo.

pause
