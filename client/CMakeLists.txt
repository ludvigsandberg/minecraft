cmake_minimum_required(VERSION 3.21)
project(client C)

include(../cmake/Template.cmake)

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include)

template_instantiate(
    HEADER  "${CMAKE_SOURCE_DIR}/common/template/arr.h"
    SOURCE  "${CMAKE_SOURCE_DIR}/common/template/arr.c"
    SUBDIR  "client"

    ITEMS
        "u32"               "u32"               "common/types.h"
        "f32"               "f32"               "common/types.h"
        "glyph"             "glyph_t"           "client/glyph.h"
        "chunk_job_ptr"     "chunk_job_t *"     "client/chunk_job.h"
        "chunk_result_ptr"  "chunk_result_t *"  "client/chunk_result.h")

set(SOURCES
    src/camera.c
    src/chunk.c
    src/gl.c
    src/gui.c
    src/lighting.c
    src/main.c
    src/sky.c
    src/world.c
    src/bitmap.c
    src/player.c
    src/linux.c
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src/arr_u32.c
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src/arr_f32.c
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src/arr_glyph.c
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src/arr_chunk_job_ptr.c
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src/arr_chunk_result_ptr.c)

add_executable(client ${SOURCES})

set_target_properties(client PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON)

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_BINARY_DIR}/generated/inc)

target_link_libraries(client PRIVATE common glad m X11 GL)

target_compile_options(client PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
    -Wshadow
    -Wunused
    -Wswitch-enum
    -Wno-error=unused-parameter)
