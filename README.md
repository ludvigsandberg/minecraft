# Minecraft

Cross-platform C99/OpenGL Minecraft.

## Build Dependencies

- [Git](https://git-scm.com/downloads)
- [Docker](https://www.docker.com)

## Build

```sh
git clone https://github.com/ludvigsandberg/minecraft
cd minecraft

# Linux
docker build --target=linux-export -t minecraft --output out/ .

# Windows
docker build --target=windows-export -t minecraft --output out/ .
```

Add `--build-arg BUILD_TYPE=Debug` to include debug symbols.

## Run

Locate the binary in `out/` and run it locally on your machine.

## Developers

You will want to setup a dev environment where docker doesn't recompile everything every time you build.
Use your editor of choice. 
On Windows I recommend [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) and [WinDbg](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger).

1. Configure CMake.
    ```sh
    cmake -B build -S .
    ```

2. Build
    ```sh
    # Windows

    # Build container, run this once
    docker build -t minecraft --target=windows-install .

    # Build executable
    docker run --rm -v "$(pwd)":/app -w /app minecraft sh -c "cmake -B build-docker -S . -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_C_FLAGS='-static -static-libgcc' -DCMAKE_BUILD_TYPE=Debug && cmake --build build-docker"
    ```

3. Run
    ```sh
    # Windows
    ./build-docker/minecraft.exe
    ```

Contributors are welcome to:
- Add macOS support to the Dockerfile.