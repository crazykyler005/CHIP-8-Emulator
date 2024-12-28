This is a functional c++ Chip-8 emulator/interrupter that uses SLD2 for rendering, keyboard input and sound and ImGui for creating the menubar and the file dialog window.

## Features
- Ability to switch between using different chip8 interpreters (original, chip8-48, legacy superchip (v1.0 and 1.1) and superchip modern, and xo-chip)
- Ability to maginify the resolution by x amount of the native
- Adjust the amount of opcodes ran per second
- Step through commands while paused (ctrl-RIGHT)
- Pause/Resume (ctrl-p)
- Reset option (ctrl-r)
- Select a color scheme (default white and black)
- Save/Load states
- Ability to maginify the resolution by x amount of the native
  
## Planned features
- Program in sound (read audio samples) for the XOChip
- Ability to enable/disable opcode quirks that are used between various versions
- Save/Load states

## Requirements
You need install sdl2 to get this to compile

You can download the latest version [here](https://github.com/libsdl-org/SDL/releases/) or install it directly on linux the following command:
```
apt-get install libsdl2-dev
```

You also need ImGuiFileDialog which can be found [here](https://github.com/aiekick/ImGuiFileDialog) and you'll need to download the following files from it and put them into the lib/imgui submodule folder
```
ImGuiFileDialog.cpp
ImGuiFileDialog.h
ImGuiFileDialogConfig.cpp
```

## Running the application
To run the application you need to first generate the make file but running the following terminal commands in the root directory of the repo

```
mkdir build
cd build
cmake ..
```

Next build the application

```
make
```

Run the executable

```
./chip8
```
