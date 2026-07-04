cmake_minimum_required(VERSION 3.21)
project(client C)

include(FetchContent)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_VIDEO ON CACHE BOOL "" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
set(SDL_X11_XSCRNSAVER OFF CACHE BOOL "" FORCE)
set(SDL_X11_XCURSOR OFF CACHE BOOL "" FORCE)
set(SDL_X11_XINPUT OFF CACHE BOOL "" FORCE)
set(SDL_X11_XFIXES OFF CACHE BOOL "" FORCE)
set(SDL_X11_XTEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SDL3 GIT_REPOSITORY https://github.com/libsdl-org/SDL GIT_TAG release-3.4.8)

FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb GIT_TAG master)

FetchContent_MakeAvailable(SDL3 stb)

add_library(stb_headers INTERFACE)
target_include_directories(stb_headers INTERFACE ${stb_SOURCE_DIR})

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

add_executable(client
    src/camera.c
    src/chunk.c
    src/gl.c
    src/gui.c
    src/lighting.c
    src/main.c
    src/sky.c
    src/world.c)

set_target_properties(client PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(client PRIVATE SDL3::SDL3 glad stb_headers)

include(Template)

target_template_instantiate(client
    HEADER "${CMAKE_SOURCE_DIR}/common/template/arr.h"
    SOURCE "${CMAKE_SOURCE_DIR}/common/template/arr.c"
    NAME "u32"
    TYPE "u32"
    TYPE_INCLUDE "common/types.h")

target_template_instantiate(client
    HEADER "${CMAKE_SOURCE_DIR}/common/template/arr.h"
    SOURCE "${CMAKE_SOURCE_DIR}/common/template/arr.c"
    NAME "f32"
    TYPE "f32"
    TYPE_INCLUDE "common/types.h")

target_template_instantiate(client
    HEADER "${CMAKE_SOURCE_DIR}/common/template/arr.h"
    SOURCE "${CMAKE_SOURCE_DIR}/common/template/arr.c"
    NAME "glyph"
    TYPE "glyph_t"
    TYPE_INCLUDE "client/glyph.h")

target_template_instantiate(client
    HEADER "${CMAKE_SOURCE_DIR}/common/template/arr.h"
    SOURCE "${CMAKE_SOURCE_DIR}/common/template/arr.c"
    NAME "chunk_job_ptr"
    TYPE "chunk_job_t *"
    TYPE_INCLUDE "client/chunk_job.h")

target_template_instantiate(client
    HEADER "${CMAKE_SOURCE_DIR}/common/template/arr.h"
    SOURCE "${CMAKE_SOURCE_DIR}/common/template/arr.c"
    NAME "chunk_result_ptr"
    TYPE "chunk_result_t *"
    TYPE_INCLUDE "client/chunk_result.h")

target_compile_options(client PRIVATE $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
    -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wunused -Wno-error=unused-parameter -Wswitch-enum -pedantic-errors
    >)
