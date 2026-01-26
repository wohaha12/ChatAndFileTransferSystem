#!/bin/bash

echo "========================================"
echo "Deployment Script"
echo "========================================"
echo ""

BUILD_DIR="build"
DEPLOY_DIR="deploy"

if [ ! -d "$BUILD_DIR/bin" ]; then
    echo "Error: Build directory not found!"
    echo "Please run build.sh first."
    exit 1
fi

if [ -d "$DEPLOY_DIR" ]; then
    echo "Removing old deployment directory..."
    rm -rf "$DEPLOY_DIR"
fi

echo "Creating deployment directory..."
mkdir -p "$DEPLOY_DIR/bin"
mkdir -p "$DEPLOY_DIR/config"
mkdir -p "$DEPLOY_DIR/logs"
mkdir -p "$DEPLOY_DIR/data"

echo "Copying executables..."
cp "$BUILD_DIR/bin/Client" "$DEPLOY_DIR/bin/"
cp "$BUILD_DIR/bin/MetaServer" "$DEPLOY_DIR/bin/"
cp "$BUILD_DIR/bin/StorageServer" "$DEPLOY_DIR/bin/"

if [ -d "Config" ]; then
    echo "Copying configuration files..."
    cp Config/*.conf "$DEPLOY_DIR/config/"
fi

if [ -d "Scripts" ]; then
    echo "Copying scripts..."
    cp Scripts/*.sh "$DEPLOY_DIR/"
fi

echo ""
echo "========================================"
echo "Deployment completed successfully!"
echo "========================================"
echo ""
echo "Deployment directory: $DEPLOY_DIR"
echo ""
