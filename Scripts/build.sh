#!/bin/bash

echo "========================================"
echo "ChatAndFileTransferSystem Build Script"
echo "========================================"
echo ""

BUILD_DIR="build"
BUILD_TYPE="Release"

if [ "$1" = "debug" ] || [ "$1" = "Debug" ]; then
    BUILD_TYPE="Debug"
fi

echo "Build Type: $BUILD_TYPE"
echo ""

if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

if [ $? -ne 0 ]; then
    echo ""
    echo "CMake configuration failed!"
    exit 1
fi

echo ""
echo "Building project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo ""
    echo "Build failed!"
    exit 1
fi

echo ""
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo ""
echo "Executables are in: $BUILD_DIR/bin"
echo ""
