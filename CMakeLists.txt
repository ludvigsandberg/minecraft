cmake_minimum_required(VERSION 3.21)
project(minecraft C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)
