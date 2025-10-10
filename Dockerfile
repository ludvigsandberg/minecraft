FROM alpine:latest AS windows-install

RUN apk add --no-cache \
    mingw-w64-gcc \
    cmake \
    build-base

WORKDIR /app

FROM windows-install AS windows-build

COPY CMakeLists.txt ./
COPY lib/ ./lib/
COPY inc/ ./inc/
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
