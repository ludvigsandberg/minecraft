cmake_minimum_required(VERSION 3.21)

include(FetchContent)

project(shared C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.c
)

add_library(shared STATIC ${SOURCES})

target_include_directories(shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_compile_options(shared PRIVATE
    $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
        -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wunused -Wno-error=unused-parameter -Wswitch-enum -pedantic-errors
    >
)
