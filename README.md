# Darkcraft

![C](https://img.shields.io/badge/language-C99-lightgrey)
![OpenGL](https://img.shields.io/badge/graphics-OpenGL-blue)

Cross-platform Minecraft-esque game in C99/OpenGL

![Screenshot](docs/Minecraft%202025-12-12%2014_03_16.png)

## Features

- Cubic chunks
- Infinite terrain generation
- Dynamic lighting

## Dependencies

- OpenGL 4.5
- C99 compiler
- CMake

## Build

```sh
git clone --recursive https://github.com/ludvigsandberg/darkcraft
cd darkcraft
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Replace `Release` with `Debug` to include debug symbols

## Run

Locate the binary in `build/` and run it
