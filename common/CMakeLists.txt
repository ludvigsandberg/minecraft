cmake_minimum_required(VERSION 3.21)
project(common C)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.c)

add_library(common STATIC ${SOURCES})

set_target_properties(common PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_compile_options(common PRIVATE $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
    -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wunused -Wno-error=unused-parameter -Wswitch-enum -pedantic-errors
    >)
