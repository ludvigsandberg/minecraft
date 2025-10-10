# Minecraft

Cross-platform C99/OpenGL Minecraft.

## Build Dependencies

- [Git](https://git-scm.com/downloads)
- [Docker](https://www.docker.com)

## Build

```sh
git clone https://github.com/ludvigsandberg/minecraft
cd minecraft

# Windows
docker build --target=windows-export -t minecraft --output out/ .
```

Add `--build-arg BUILD_TYPE=Debug` to include debug symbols.

## Run

Locate the binary in `out/` and run it locally on your machine.

## Developers

### Dev Environment

You want to setup an iterative dev environment so that docker doesn't recompile everything when you build.
Use your editor of choice. 
On Windows I strongly recommend using WSL.

1. Configure CMake. This will generate `build/compile_commands.json` which clangd requires.
If you aren't using clangd you might be able to skip this step.
    ```sh
    cmake -B build -S .
    ```

2. Build
    ```sh
    # Windows

    # Build container, run once
    docker build -t minecraft --target=windows-install .

    # Run container with bind mount to build executable
    docker run --rm -v "$(pwd)":/app -w /app minecraft sh -c "cmake -B build-docker -S . -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_C_FLAGS='-static -static-libgcc' -DCMAKE_BUILD_TYPE=Debug && cmake --build build-docker"
    ```

3. Run
    ```sh
    # Windows
    ./build-docker/minecraft.exe
    ```

## Contribute

Contributors are welcome to:
- Add Linux and macOS Docker support.