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

In Windows you need to download it from [here](https://github.com/libsdl-org/SDL/releases). Check what the latestest stable release version is of SDL2 and download `SDL2-devel-2.x.x-VC.zip` from the "Assests" dropdown. It's recommend to extract the contents into C:/ and rename the base folder from SDL2-2.x.x to SDL2

You also need ImGuiFileDialog which can be found [here](https://github.com/aiekick/ImGuiFileDialog) and you'll need to download the following files from it and put them into the lib/imgui submodule folder
```
ImGuiFileDialog.cpp
ImGuiFileDialog.h
ImGuiFileDialogConfig.cpp
```
Also include the `direct/` folder if your compiling on Windows

## Running the application
### In Linux
To run the application you need to first generate the executable using the following make command:
```
make build
```

Next run the executable
```
.build/chip8
```
or build and run
```
make run
```

### In Windows
Open the .env and confirm that the SDL2 directories in there exist and give new paths if need be
Next, all you need do is run the bat file from the root directory
```
./build.bat
```
or
```
./build.bat Debug
```

There should now be a `build` directory created in the root directory and within that should be a `Release` or `Debug` folder that contains the executable `chip8.exe`