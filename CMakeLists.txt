cmake_minimum_required(VERSION 3.21)
project(minecraft C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FLAGS
    -std=c90
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -pedantic-errors
    -Wformat=2
    -Wconversion
    -Wsign-conversion
    -Wdouble-promotion
    -Wfloat-equal
    -Wnull-dereference
    -Wshadow
    -Wunused
    -Wswitch-enum
    -Wcast-qual
    -Wcast-align
    -Wwrite-strings
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wold-style-definition
    -Wundef
    -Wredundant-decls
    -Wvla
    -Wno-error=unused-parameter)

###############################################################################
# Client
###############################################################################

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include)

add_executable(client 
    src/client_main.c
    src/camera.c
    src/chunk.c
    src/opengl.c
    src/gui.c
    src/sky.c
    src/world.c
    src/file_io.c
    src/system.c
    src/vector.c
    src/matrix.c)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(client PRIVATE glad m X11 GL)
target_compile_options(client PRIVATE ${FLAGS})

###############################################################################
# Server
###############################################################################

add_executable(server 
    src/server_main.c
    src/vector.c
    src/matrix.c)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server PRIVATE m)
target_compile_options(server PRIVATE ${FLAGS})