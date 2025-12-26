# Backend Dockerfile for LLVM Obfuscator
# This should be built from the project root
FROM node:20-slim AS base

# Install system dependencies including LLVM/Clang with dev packages
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    llvm-dev \
    libllvm-dev \
    build-essential \
    cmake \
    ninja-build \
    git \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Build stage for obfuscator
FROM base AS obfuscator-builder
WORKDIR /build

# Copy source files needed for building obfuscator
COPY include ./include
COPY src/ObfuscationPass.cpp ./src/
COPY tools/llvm-obfuscator.cpp ./tools/
COPY CMakeLists.txt ./

# Find LLVM CMake config and build obfuscator
RUN set -e && \
    # Try to find LLVM CMake config
    LLVM_CMAKE=$(find /usr -name "LLVMConfig.cmake" 2>/dev/null | head -1) && \
    if [ -z "$LLVM_CMAKE" ]; then \
        echo "Warning: LLVMConfig.cmake not found, trying default paths"; \
        LLVM_DIR="/usr/lib/llvm-*/lib/cmake/llvm" || true; \
    else \
        LLVM_DIR=$(dirname "$LLVM_CMAKE"); \
        echo "Found LLVM at: $LLVM_DIR"; \
    fi && \
    # Build obfuscator
    mkdir -p build && cd build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DLLVM_OBFUSCATOR_USE_SYSTEM_LLVM=ON \
        ${LLVM_DIR:+-DLLVM_DIR="$LLVM_DIR"} \
        -DCMAKE_VERBOSE_MAKEFILE=ON && \
    cmake --build . --target llvm-obfuscator -j$(nproc) && \
    if [ -f bin/llvm-obfuscator ]; then \
        cp bin/llvm-obfuscator /tmp/llvm-obfuscator && \
        chmod +x /tmp/llvm-obfuscator && \
        echo "Build successful!"; \
    else \
        echo "Error: llvm-obfuscator binary not found!"; \
        ls -la bin/ || true; \
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


