cmake_minimum_required(VERSION 3.21)
project(common C)

set(SOURCES
    src/mem.c
    src/vec.c
    src/mat.c)

add_library(common STATIC ${SOURCES})

set_target_properties(common PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_compile_options(common PRIVATE 
    -Wall 
    -Wextra 
    -Wpedantic 
    -Werror 
    -Wconversion 
    -Wshadow 
    -Wunused 
    -Wno-error=unused-parameter 
    -Wswitch-enum 
    -pedantic-errors)
