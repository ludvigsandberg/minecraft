cmake_minimum_required(VERSION 3.21)
project(minecraft C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.c
)

add_executable(minecraft ${SOURCES})

target_include_directories(minecraft PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

add_subdirectory(vendor/x)

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

add_library(linmath INTERFACE)
target_include_directories(linmath INTERFACE vendor/linmath.h)
target_compile_options(linmath INTERFACE -w)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE vendor/stb)

target_link_libraries(minecraft PRIVATE SDL3::SDL3 x glad linmath stb)

target_compile_options(minecraft PRIVATE
    $<$<C_COMPILER_ID:MSVC>:
        /W4 /WX /permissive-
    >

    $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
        -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wunused -Wno-error=unused-parameter -Wswitch-enum -pedantic-errors
    >
)
