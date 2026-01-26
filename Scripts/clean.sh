#!/bin/bash

echo "========================================"
echo "Clean Build Script"
echo "========================================"
echo ""

BUILD_DIR="build"

if [ -d "$BUILD_DIR" ]; then
    echo "Removing build directory..."
    rm -rf "$BUILD_DIR"
    echo "Build directory removed successfully."
else
    echo "Build directory does not exist."
fi

echo ""
echo "Clean completed!"
echo ""
