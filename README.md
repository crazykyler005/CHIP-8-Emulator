This is a functional c++ Chip-8 emulator/interrupter that uses SLD2 for rendering, keyboard input and sound and ImGui for creating the menubar and the file dialog window. 

## Planned Menubar features
- Ability to switch between using different chip8 languages (chip8 original, chip8-48, super-chip8)
- Ability to enable/disable opcode and opcode differences that are used between various versions
- ~~Adjust the amount of opcodes ran per second~~
- ~~Select and load a program/game from a file~~
- ~~Pause option~~
- ~~Reset option~~
- ~~Select a color scheme (default white and black)~~
- Save/Load states
- ~~Ability maginify the resolution by x amount of the native~~

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