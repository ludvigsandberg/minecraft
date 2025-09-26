@echo off
set OUT_DIR=out\windows
if not exist %OUT_DIR% mkdir %OUT_DIR%

set BUILD_ARG=
set BUILD_TYPE=Debug
if /i "%1"=="Release" (
    set BUILD_ARG=--build-arg BUILD_TYPE=Release
    set BUILD_TYPE=Release
)

docker build %BUILD_ARG% --target windows-build -t minecraft-windows .

for /f "delims=" %%i in ('docker create minecraft-windows') do set CID=%%i
docker cp %CID%:/out/windows/minecraft.exe %OUT_DIR%\minecraft.exe
docker rm %CID%