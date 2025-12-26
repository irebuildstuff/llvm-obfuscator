# Backend Dockerfile for LLVM Obfuscator
# This should be built from the project root
FROM node:20-slim AS base

# Install system dependencies including LLVM/Clang with dev packages
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    llvm-dev \
    build-essential \
    cmake \
    ninja-build \
    git \
    pkg-config \
    libc++-dev \
    libc++abi-dev \
    && rm -rf /var/lib/apt/lists/*

# Build stage for obfuscator
FROM base AS obfuscator-builder
WORKDIR /build

# Copy source files needed for building obfuscator
COPY include ./include
COPY src/ObfuscationPass.cpp ./src/
COPY tools/llvm-obfuscator.cpp ./tools/
COPY CMakeLists.txt ./
# Copy optional files if they exist (for CPack)
COPY cmake/ ./cmake/ 2>/dev/null || true
COPY LICENSE ./ 2>/dev/null || true
COPY README.md ./ 2>/dev/null || true

# Diagnostic: Check system and LLVM
RUN echo "=== System Info ===" && \
    cat /etc/os-release && \
    echo "" && \
    echo "=== LLVM Packages ===" && \
    dpkg -l | grep -i llvm && \
    echo "" && \
    echo "=== Finding LLVM Files ===" && \
    find /usr -name "LLVMConfig.cmake" 2>/dev/null && \
    find /usr/lib -type d -name "llvm*" 2>/dev/null | head -10

# Find LLVM CMake config
RUN LLVM_CMAKE=$(find /usr -name "LLVMConfig.cmake" 2>/dev/null | head -1) && \
    if [ -n "$LLVM_CMAKE" ]; then \
        LLVM_DIR=$(dirname "$LLVM_CMAKE") && \
        echo "Found LLVM at: $LLVM_DIR" && \
        echo "$LLVM_DIR" > /tmp/llvm_dir.txt; \
    else \
        echo "LLVMConfig.cmake not found, trying common paths..." && \
        for path in /usr/lib/llvm-*/lib/cmake/llvm /usr/lib/llvm*/lib/cmake/llvm; do \
            if [ -d "$path" ] 2>/dev/null; then \
                echo "Using: $path" && \
                echo "$path" > /tmp/llvm_dir.txt && \
                break; \
            fi; \
        done; \
    fi && \
    if [ ! -f /tmp/llvm_dir.txt ]; then \
        echo "WARNING: No LLVM found, will try without explicit path" && \
        echo "" > /tmp/llvm_dir.txt; \
    fi

# Configure CMake
RUN mkdir -p build && cd build && \
    LLVM_DIR=$(cat /tmp/llvm_dir.txt) && \
    if [ -n "$LLVM_DIR" ]; then \
        CMAKE_ARGS="-DLLVM_DIR=$LLVM_DIR"; \
    else \
        CMAKE_ARGS=""; \
    fi && \
    echo "Running: cmake .. -DCMAKE_BUILD_TYPE=Release -DLLVM_OBFUSCATOR_USE_SYSTEM_LLVM=ON $CMAKE_ARGS" && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DLLVM_OBFUSCATOR_USE_SYSTEM_LLVM=ON \
        $CMAKE_ARGS \
        -DCMAKE_VERBOSE_MAKEFILE=ON

# Build obfuscator
RUN cd build && \
    echo "=== Compiling ===" && \
    cmake --build . --target llvm-obfuscator -j$(nproc) || \
    (echo "=== Parallel build failed, retrying single-threaded ===" && \
     cmake --build . --target llvm-obfuscator -j1)

# Verify and copy binary
RUN cd build && \
    if [ -f bin/llvm-obfuscator ]; then \
        echo "=== Build successful! ===" && \
        cp bin/llvm-obfuscator /tmp/llvm-obfuscator && \
        chmod +x /tmp/llvm-obfuscator && \
        ls -lh /tmp/llvm-obfuscator && \
        file /tmp/llvm-obfuscator; \
    else \
        echo "=== ERROR: Binary not found ===" && \
        echo "Build directory contents:" && \
        find . -name "*.o" -o -name "*.a" -o -name "*obfuscator*" | head -20 && \
        echo "CMake error log:" && \
        cat CMakeFiles/CMakeError.log 2>/dev/null | tail -50 || echo "No error log" && \
        exit 1; \
    fi

# Backend application stage
FROM base AS backend
WORKDIR /app

# Copy package files
COPY backend/package*.json ./

# Install Node.js dependencies
RUN npm install --production

# Copy backend application code
COPY backend/ ./

# Copy obfuscator binary from builder
COPY --from=obfuscator-builder /tmp/llvm-obfuscator /app/llvm-obfuscator
RUN chmod +x /app/llvm-obfuscator

# Create necessary directories
RUN mkdir -p uploads output jobs

# Set environment variables
ENV NODE_ENV=production
ENV PORT=3001
ENV CLANG_PATH=clang
ENV OBFUSCATOR_PATH=/app/llvm-obfuscator

# Expose port
EXPOSE 3001

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
  CMD node -e "require('http').get('http://localhost:3001/health', (r) => {process.exit(r.statusCode === 200 ? 0 : 1)})"

# Start server
CMD ["node", "src/server.js"]


