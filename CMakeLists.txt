cmake_minimum_required(VERSION 3.10)
project(CHIP-8_Emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

# Include GTKmm directories
include_directories(${GTKMM_INCLUDE_DIRS})

# Add compiler flags (in case include directories are not automatically applied)
add_compile_options(${GTKMM_CFLAGS_OTHER})

add_executable(test
    src/main.cpp
    src/window.cpp
    src/screen.cpp
    src/chip8.cpp
    src/menubar.cpp
    src/helper_functions.cpp
)

# Link GTKmm libraries
target_link_libraries(test ${GTKMM_LIBRARIES})

# Resolve link directories and libraries if needed
link_directories(${GTKMM_LIBRARY_DIRS})