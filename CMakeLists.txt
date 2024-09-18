cmake_minimum_required(VERSION 3.10)
project(CHIP-8_Emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add SDL2 (this will find your SDL2 installation)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

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
    # ${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

# Include ImGui headers
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui)
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui/backends)

add_executable(chip8
	${IMGUI_SRC}
    src/main.cpp
    src/window.cpp
    src/screen.cpp
    src/chip8.cpp
    src/menubar.cpp
    src/helper_functions.cpp
)

# Link GTKmm libraries
target_link_libraries(chip8 ${SDL2_LIBRARIES} ${GTKMM_LIBRARIES})

# Resolve link directories and libraries if needed
link_directories(${GTKMM_LIBRARY_DIRS})