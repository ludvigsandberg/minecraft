# minecraft

![C](https://img.shields.io/badge/language-C99-lightgrey)
![OpenGL](https://img.shields.io/badge/graphics-OpenGL-blue)

Minecraft in C99/OpenGL

![Screenshot 1](docs/minecraft%202026-01-18%2010_56_48.png)
![Screenshot 2](docs/Minecraft%202025-12-12%2017_00_05.png)

## Features

- Cross platform
- Place and remove blocks
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
