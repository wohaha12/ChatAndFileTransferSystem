#!/bin/bash

echo "========================================"
echo "Test Script"
echo "========================================"
echo ""

BUILD_DIR="build"

if [ ! -f "$BUILD_DIR/bin/tests" ]; then
    echo "Error: Tests executable not found!"
    echo "Please run build.sh first."
    exit 1
fi

echo "Running tests..."
cd "$BUILD_DIR/bin"
./tests

if [ $? -ne 0 ]; then
    echo ""
    echo "Tests failed!"
    exit 1
fi

echo ""
echo "========================================"
echo "All tests passed!"
echo "========================================"
echo ""
