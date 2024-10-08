cmake_minimum_required(VERSION 3.10)
project(CHIP-8_Emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OS Detection
if(WIN32)
    find_package(SDL2 REQUIRED PATHS "C:/path/to/SDL2")
    include_directories(${SDL2_INCLUDE_DIRS})
else()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

find_package(PkgConfig REQUIRED)

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
	src/Chip8/superChip.cpp

    src/menubar.cpp
    src/helper_functions.cpp
)

# Link SDL2 libraries based on OS
if(WIN32)
    target_link_libraries(chip8 ${SDL2_LIBRARIES} SDL2main)
else()
    target_link_libraries(chip8 ${SDL2_LIBRARIES})
endif()