# Compile for Linux
FROM ubuntu:22.04 AS linux-build

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    gdb \
    libgl1-mesa-dev \
    libx11-dev \
    libxi-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxkbcommon-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY CMakeLists.txt ./
COPY inc/ ./inc/
COPY src/ ./src/
COPY lib/ ./lib/

WORKDIR /app/build

ARG BUILD_TYPE=Release
RUN cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE .. && make

# Export Linux binary
FROM scratch AS linux-export
COPY --from=linux-build /app/build/minecraft /

# Cross-compile for Windows using MinGW
FROM alpine:latest AS windows-build

RUN apk add --no-cache \
    mingw-w64-gcc \
    cmake \
    build-base

WORKDIR /app

COPY CMakeLists.txt ./
COPY inc/ ./inc/
COPY src/ ./src/
COPY lib/ ./lib/

WORKDIR /app/build

ARG BUILD_TYPE=Release
RUN cmake \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_SYSTEM_NAME="Windows" \
    -DCMAKE_C_COMPILER="x86_64-w64-mingw32-gcc" \
    -DCMAKE_C_FLAGS="-static -static-libgcc" \
    .. && make

# Export Windows binary
FROM scratch AS windows-export
COPY --from=windows-build /app/build/minecraft.exe /