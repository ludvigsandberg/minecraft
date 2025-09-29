cmake_minimum_required(VERSION 3.21)
project(Minecraft C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
if(UNIX AND NOT APPLE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(lib/glfw)
add_subdirectory(lib/meta)

add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC SYSTEM lib/glad/include)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    inc/*.h
    src/*.c
)

add_executable(minecraft ${SOURCES})

target_include_directories(minecraft PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(minecraft PRIVATE glfw meta glad)

target_compile_options(minecraft PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
    -Wshadow
    -Wunused
    -Wno-error=unused-parameter
    -Wswitch-enum
    -pedantic-errors
)

