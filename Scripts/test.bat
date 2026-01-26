@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Test Script
echo ========================================
echo.

set BUILD_DIR=build

if not exist "%BUILD_DIR%\bin\tests.exe" (
    echo Error: Tests executable not found!
    echo Please run build.bat first.
    pause
    exit /b 1
)

echo Running tests...
cd "%BUILD_DIR%\bin"
tests.exe

if errorlevel 1 (
    echo.
    echo Tests failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo All tests passed!
echo ========================================
echo.

pause
