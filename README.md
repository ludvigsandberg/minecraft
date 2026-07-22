# Minecraft

Minecraft in C89/OpenGL

![Screenshot 1](docs/minecraft%202026-01-18%2010_56_48.png)
![Screenshot 2](docs/Screenshot%202026-07-18%20152955.png)

## Features

- Infinite terrain generation
- Multiplayer (WIP)

## Dependencies

- Linux/WSL
- OpenGL 3.3
- GCC/Clang
- CMake

## Build

```sh
git clone https://github.com/ludvigsandberg/minecraft
cd minecraft
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Run

Locate the client in `build/` and run it from project root.

## Contribute

PRs are welcome. Format using clang-format.
