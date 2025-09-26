cmake_minimum_required(VERSION 3.21)
project(Minecraft C)

# -------------------------------
# Compiler / Platform setup
# -------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring for Windows cross-compile")
    
    # Prefer MinGW GCC if available
    if(NOT CMAKE_C_COMPILER)
        set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
    endif()
    if(NOT CMAKE_CXX_COMPILER)
        set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    endif()

    # Static linking for Windows GUI
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

# -------------------------------
# Add subdirectories
# -------------------------------
add_subdirectory(lib/glfw)
add_subdirectory(lib/meta)

# -------------------------------
# Collect sources
# -------------------------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

# -------------------------------
# Add executable
# -------------------------------
add_executable(minecraft ${SOURCES} lib/glad/src/glad.c)

set_source_files_properties(lib/glad/src/glad.c PROPERTIES
    COMPILE_FLAGS "-w"
)

set_target_properties(minecraft PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    CMAKE_EXPORT_COMPILE_COMMANDS ON
)

# Include directories
target_include_directories(minecraft PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    SYSTEM lib/glad/include
)

# Link libraries
target_link_libraries(minecraft PRIVATE glfw meta)

# Compile options
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

