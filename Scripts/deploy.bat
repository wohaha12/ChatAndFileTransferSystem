@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Deployment Script
echo ========================================
echo.

set BUILD_DIR=build
set DEPLOY_DIR=deploy

if not exist "%BUILD_DIR%\bin" (
    echo Error: Build directory not found!
    echo Please run build.bat first.
    pause
    exit /b 1
)

if exist "%DEPLOY_DIR%" (
    echo Removing old deployment directory...
    rmdir /s /q "%DEPLOY_DIR%"
)

echo Creating deployment directory...
mkdir "%DEPLOY_DIR%"
mkdir "%DEPLOY_DIR%\bin"
mkdir "%DEPLOY_DIR%\config"
mkdir "%DEPLOY_DIR%\logs"
mkdir "%DEPLOY_DIR%\data"

echo Copying executables...
copy "%BUILD_DIR%\bin\Client.exe" "%DEPLOY_DIR%\bin\" >nul
copy "%BUILD_DIR%\bin\MetaServer.exe" "%DEPLOY_DIR%\bin\" >nul
copy "%BUILD_DIR%\bin\StorageServer.exe" "%DEPLOY_DIR%\bin\" >nul

if exist "Config" (
    echo Copying configuration files...
    xcopy "Config\*.conf" "%DEPLOY_DIR%\config\" /Y >nul
)

if exist "Scripts" (
    echo Copying scripts...
    xcopy "Scripts\*.bat" "%DEPLOY_DIR%\" /Y >nul
)

echo.
echo ========================================
echo Deployment completed successfully!
echo ========================================
echo.
echo Deployment directory: %DEPLOY_DIR%
echo.

pause
