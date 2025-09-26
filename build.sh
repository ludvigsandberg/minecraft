#!/bin/bash
set -e

OUT_DIR="./out/linux"
mkdir -p "$OUT_DIR"

BUILD_ARG=""
BUILD_TYPE="Debug"
if [[ "$1" == "Release" ]]; then
    BUILD_ARG="--build-arg BUILD_TYPE=Release"
    BUILD_TYPE="Release"
fi

docker build $BUILD_ARG --target linux-build -t minecraft-linux .

CID=$(docker create minecraft-linux)
docker cp "$CID":/out/linux/minecraft "$OUT_DIR/minecraft"
docker rm "$CID"

