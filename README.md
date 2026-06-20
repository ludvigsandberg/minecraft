# minecraft

![C](https://img.shields.io/badge/language-C89-lightgrey)
![OpenGL](https://img.shields.io/badge/graphics-OpenGL-blue)

Minecraft in C89/OpenGL

![Screenshot 1](docs/minecraft%202026-01-18%2010_56_48.png)
![Screenshot 2](docs/Minecraft%202025-12-12%2017_00_05.png)
![Screenshot 3](docs/2026-05-06.png)

## Features

- Infinite terrain generation
- Multiplayer (WIP)

## Dependencies

- Linux/WSL
- OpenGL 4.5
- C89 compiler
- CMake

## Build

```sh
git clone https://github.com/ludvigsandberg/minecraft
cd minecraft
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release # or Debug
cmake --build build --config Release # or Debug
```

## Run

Locate the client in `build/` and run it
