cmake_minimum_required(VERSION 3.21)
project(server C)

add_executable(server
    src/helloworld.c)

set_target_properties(server PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(server PRIVATE common)

target_compile_options(server PRIVATE $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
    -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wunused -Wno-error=unused-parameter -Wswitch-enum -pedantic-errors
    >)
