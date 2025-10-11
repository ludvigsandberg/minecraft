FROM ubuntu:24.04 AS linux-install

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxext-dev \
    libxfixes-dev \
    libxkbfile-dev \
    libxss-dev \
    libudev-dev \
    libdbus-1-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

FROM linux-install AS linux-build

COPY CMakeLists.txt ./ 
COPY lib/ ./lib/
COPY src/ ./src/

WORKDIR /app/build

ARG BUILD_TYPE=Release
RUN cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE .. && make

FROM scratch AS linux-export
COPY --from=linux-build /app/build/minecraft /

FROM alpine:latest AS windows-install

RUN apk add --no-cache \
    mingw-w64-gcc \
    cmake \
    build-base

WORKDIR /app

FROM windows-install AS windows-build

COPY CMakeLists.txt ./
COPY lib/ ./lib/
COPY src/ ./src/

WORKDIR /app/build

ARG BUILD_TYPE=Release
RUN cmake \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_SYSTEM_NAME="Windows" \
    -DCMAKE_C_COMPILER="x86_64-w64-mingw32-gcc" \
    -DCMAKE_C_FLAGS="-static -static-libgcc" \
    .. && make

FROM scratch AS windows-export
COPY --from=windows-build /app/build/minecraft.exe /
