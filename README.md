# Minecraft

High-concurrency multiplayer Minecraft in C89 with spacial sharding

![Video screenshot](docs/Screenshot%202026-07-26%20171316.png)
![Screenshot](docs/minecraft%202026-01-18%2010_56_48.png)

## Dependencies

- Linux
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

Singleplayer
```sh
./build/client
```

Connect to server
```sh
./build/client <server-address> <port>
```

Launch server
```sh
./build/server <port>
```