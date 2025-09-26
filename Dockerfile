# -----------------------
# Stage 1: Linux build
# -----------------------
FROM ubuntu:22.04 AS linux-build

# Install build tools
RUN apt-get update && apt-get install -y \
    clang cmake ninja-build git build-essential

WORKDIR /app
COPY . .

ARG BUILD_TYPE=Release

# Configure and build Linux executable
RUN cmake -B build-linux -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE . && \
    cmake --build build-linux --config $BUILD_TYPE

# Copy Linux artifact to /out/linux
RUN install -D build-linux/minecraft /out/linux/minecraft

# -----------------------
# Stage 2: Windows cross-compile with MinGW
# -----------------------
FROM ubuntu:22.04 AS windows-build

# Install MinGW cross-compiler
RUN apt-get update && apt-get install -y \
    mingw-w64 cmake ninja-build git build-essential

WORKDIR /app
COPY . .

ARG BUILD_TYPE=Release

# Configure CMake for cross-compilation to Windows
RUN cmake -B build-win -G Ninja \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DCMAKE_EXE_LINKER_FLAGS="-static -static-libgcc -static-libstdc++" \
    -DGLFW_BUILD_WAYLAND=OFF \
    -DGLFW_BUILD_X11=OFF \
    -DGLFW_BUILD_EXAMPLES=OFF \
    -DGLFW_BUILD_TESTS=OFF \
    -DGLFW_BUILD_DOCS=OFF . && \
    cmake --build build-win --config $BUILD_TYPE

# Copy Windows artifact to /out/windows
RUN install -D build-win/minecraft.exe /out/windows/minecraft.exe

# -----------------------
# Stage 3: Export artifacts
# -----------------------
FROM scratch AS export

# Copy Linux and Windows artifacts
COPY --from=linux-build /out/linux /out/linux
COPY --from=windows-build /out/windows /out/windows
