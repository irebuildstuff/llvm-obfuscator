#!/bin/bash
# Build script for obfuscator binary (Linux) to be used in Docker

set -e

echo "Building LLVM Obfuscator for Linux..."

# Check if LLVM is installed
if ! command -v clang++ &> /dev/null; then
    echo "Error: clang++ not found. Please install LLVM."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Build using CMake
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target llvm-obfuscator

echo "Build complete! Binary: build/bin/llvm-obfuscator"

