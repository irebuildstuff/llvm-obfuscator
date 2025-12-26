# Backend Dockerfile for LLVM Obfuscator
# This should be built from the project root
FROM node:20-slim AS base

# Install system dependencies including LLVM/Clang
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    build-essential \
    cmake \
    ninja-build \
    git \
    && rm -rf /var/lib/apt/lists/*

# Build stage for obfuscator
FROM base AS obfuscator-builder
WORKDIR /build

# Copy source files needed for building obfuscator
COPY include ./include
COPY src/ObfuscationPass.cpp ./src/
COPY tools/llvm-obfuscator.cpp ./tools/
COPY CMakeLists.txt ./

# Build obfuscator
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --target llvm-obfuscator && \
    cp bin/llvm-obfuscator /tmp/llvm-obfuscator

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


