cmake_minimum_required(VERSION 3.10)
project(CHIP-8_Emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OS Detection
if(WIN32)
    file(READ ".env" CONFIG_CONTENT)

    # Extract the value after '=' and before the \n
    string(REGEX MATCH "SDL2_LIB_DIR=([^\r\n]+)" _ ${CONFIG_CONTENT})
    set(SDL2_LIB_DIR ${CMAKE_MATCH_1})

    string(REGEX MATCH "SDL2_INCLUDE_DIR=([^\r\n]+)" _ ${CONFIG_CONTENT})
    set(SDL2_INCLUDE_DIR ${CMAKE_MATCH_1})

    include_directories(${SDL2_INCLUDE_DIR})
    link_directories(${SDL2_LIB_DIR})

    set(SDL2_LIBRARIES "${SDL2_LIB_DIR}/SDL2.lib" "${SDL2_LIB_DIR}/SDL2main.lib")

    message(STATUS "SDL2 Include Directory: ${SDL2_INCLUDE_DIR}")
    message(STATUS "SDL2 Library Directory: ${SDL2_LIB_DIR}")
else()
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIRS})
	find_package(PkgConfig REQUIRED)
endif()

# ImGui source files
set(IMGUI_SRC
	${CMAKE_SOURCE_DIR}/lib/imgui/imgui.cpp
	${CMAKE_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
	${CMAKE_SOURCE_DIR}/lib/imgui/imgui_tables.cpp
	${CMAKE_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
	${CMAKE_SOURCE_DIR}/lib/imgui/ImGuiFileDialog.cpp
	${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_sdl2.cpp
	${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src/Chip8)

# Include ImGui headers
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui)
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui/backends)

add_executable(chip8
	${IMGUI_SRC}
	src/main.cpp
	src/window.cpp
	src/screen.cpp

	src/Chip8/chip8.cpp
	src/Chip8/chip8interpreter.cpp
	src/Chip8/superChipInterpreter.cpp
	src/Chip8/superChipLegacy.cpp
	src/Chip8/superChipModern.cpp
	src/Chip8/xoChip.cpp

	src/menubar.cpp
	src/helper_functions.cpp
)

target_link_libraries(chip8 ${SDL2_LIBRARIES})