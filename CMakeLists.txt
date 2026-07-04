cmake_minimum_required(VERSION 3.21)
project(minecraft C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(client)
add_subdirectory(server)
