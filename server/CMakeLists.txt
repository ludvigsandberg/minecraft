cmake_minimum_required(VERSION 3.21)
project(server C)

include(../cmake/Template.cmake)

template_instantiate(
    HEADER  "${CMAKE_SOURCE_DIR}/common/template/arr.h"
    SOURCE  "${CMAKE_SOURCE_DIR}/common/template/arr.c"
    SUBDIR  "server"

    ITEMS
        "u8"     "u8"       "common/types.h"
        "client" "client_t" "server/client.h")

add_executable(server
    src/main.c
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src/arr_u8.c
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src/arr_client.c)

set_target_properties(server PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON)

target_include_directories(server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_BINARY_DIR}/generated/inc)

target_link_libraries(server PRIVATE common)

target_compile_options(server PRIVATE 
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
