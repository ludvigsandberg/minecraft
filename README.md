# Minecraft
Cross-platform C99/OpenGL Minecraft.

## Build Dependencies
[Git](https://git-scm.com/downloads), [Docker](https://www.docker.com/).

## Build
```sh
git clone https://github.com/ludvigsandberg/minecraft
cd minecraft

# Linux
docker build --target=linux-export --build-arg BUILD_TYPE=Debug -t minecraft --output out/ .

# Windows
docker build --target=windows-export --build-arg BUILD_TYPE=Debug -t minecraft --output out/ .
```
Remove `--build-arg BUILD_TYPE=Debug` to build release version for improved performance.

## Run
Locate the binary in `out/` and run it.

## Dev Dependencies
VSCode + Dev Containers extension.