# Minecraft

Cross-platform Minecraft clone in C99/OpenGL

![Screenshot](docs/Minecraft%202025-12-12%2014_03_16.png)

## Features

- Cubic chunks
- Infinite terrain generation

## Dependencies

- OpenGL 4.5
- C99 compiler
- CMake

## Build

```sh
git clone --recursive https://github.com/ludvigsandberg/minecraft
cd minecraft
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Replace `Release` with `Debug` to include debug symbols

## Run

Locate the binary in `build/` and run it
