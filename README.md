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
Add `--build-arg BUILD_TYPE=Debug` to include debugging symbols.

## Run
Locate the binary in `out/` and run it.

## Developer Setup
- [VS Code](https://code.visualstudio.com)
- [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension

1. Open the project in VS Code.
2. Command Palette -> `Dev Containers: Reopen in Container`.
3. Delete `build/` if it exists and reconfigure with CMake: Command Palette -> `CMake: Configure`.
3. Command Palette -> `CMake: Build`.
3. Run and Debug -> Start Debugging (F5).