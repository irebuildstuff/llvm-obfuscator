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

# Find LLVM and build obfuscator
RUN echo "=== System Info ===" && \
    lsb_release -a 2>/dev/null || cat /etc/os-release && \
    echo "" && \
    echo "=== Checking LLVM Installation ===" && \
    dpkg -l | grep llvm || echo "No LLVM packages found" && \
    echo "" && \
    echo "=== Finding LLVM Files ===" && \
    find /usr -name "LLVMConfig.cmake" 2>/dev/null | head -10 || echo "LLVMConfig.cmake not found" && \
    find /usr/lib -type d -name "llvm*" 2>/dev/null | head -10 || echo "No LLVM directories found" && \
    echo "" && \
    LLVM_CMAKE=$(find /usr -name "LLVMConfig.cmake" 2>/dev/null | head -1) && \
    if [ -n "$LLVM_CMAKE" ]; then \
        LLVM_DIR=$(dirname "$LLVM_CMAKE") && \
        echo "Found LLVM CMake config at: $LLVM_DIR" && \
        CMAKE_LLVM_ARG="-DLLVM_DIR=$LLVM_DIR"; \
    else \
        echo "LLVMConfig.cmake not found, checking common paths..." && \
        for path in /usr/lib/llvm-*/lib/cmake/llvm /usr/lib/llvm*/lib/cmake/llvm; do \
            if [ -d "$path" ] 2>/dev/null; then \
                LLVM_DIR="$path" && \
                CMAKE_LLVM_ARG="-DLLVM_DIR=$LLVM_DIR" && \
                echo "Using LLVM directory: $LLVM_DIR" && \
                break; \
            fi; \
        done; \
        if [ -z "$CMAKE_LLVM_ARG" ]; then \
            echo "WARNING: Could not find LLVM CMake config, trying without explicit path"; \
            CMAKE_LLVM_ARG=""; \
        fi; \
    fi && \
    echo "" && \
    echo "=== Building obfuscator ===" && \
    mkdir -p build && cd build && \
    echo "Running cmake with args: -DCMAKE_BUILD_TYPE=Release -DLLVM_OBFUSCATOR_USE_SYSTEM_LLVM=ON $CMAKE_LLVM_ARG" && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DLLVM_OBFUSCATOR_USE_SYSTEM_LLVM=ON \
        $CMAKE_LLVM_ARG \
        -DCMAKE_VERBOSE_MAKEFILE=ON || (echo "CMake failed!" && cat CMakeCache.txt 2>/dev/null | head -50 && exit 1) && \
    echo "" && \
    echo "=== Compiling ===" && \
    (cmake --build . --target llvm-obfuscator -j$(nproc) 2>&1 || ( \
        echo "" && \
        echo "=== BUILD FAILED - Showing full error output ===" && \
        echo "Retrying with single thread to see all errors:" && \
        cmake --build . --target llvm-obfuscator -j1 2>&1 && \
        exit 1 \
    )) && \
    echo "" && \
    if [ -f bin/llvm-obfuscator ]; then \
        echo "=== Build successful! ===" && \
        cp bin/llvm-obfuscator /tmp/llvm-obfuscator && \
        chmod +x /tmp/llvm-obfuscator && \
        ls -lh /tmp/llvm-obfuscator && \
        file /tmp/llvm-obfuscator; \
    else \
        echo "=== ERROR: Binary not found ===" && \
        echo "Contents of build/bin:" && \
        ls -la bin/ 2>&1 || echo "bin directory doesn't exist" && \
        echo "" && \
        echo "Contents of build directory:" && \
        ls -la 2>&1 | head -30 && \
        echo "" && \
        echo "CMakeCache.txt (first 100 lines):" && \
        cat CMakeCache.txt 2>/dev/null | head -100 || echo "CMakeCache.txt not found" && \
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


