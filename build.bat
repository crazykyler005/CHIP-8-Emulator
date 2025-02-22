@echo off
setlocal

set CONFIG=Release
if "%1"=="Debug" set CONFIG=Debug

:: Read the configuration file
for /f "tokens=1,2 delims==" %%a in (.env) do (
    if "%%a"=="SDL2_LIB_DIR" set SDL2_LIB_DIR=%%b
    if "%%a"=="SDL2_INCLUDE_DIR" set SDL2_INCLUDE_DIR=%%b
    if "%%a"=="SOUND_FILE" set SOUND_FILE=%%b
)

set DIRS_FOUND=False
:: Check if the paths are set
if not defined SDL2_LIB_DIR (rem)
) else if not defined SDL2_INCLUDE_DIR (rem)
) else if not defined SOUND_FILE (rem)
) else ( set DIRS_FOUND=True )

if %DIRS_FOUND%==False (
    echo missing paths in .env
    exit /b 1
)

set DIRS_FOUND=False
:: Check if paths exist
if not exist %SDL2_LIB_DIR% (rem)
) else if not exist %SDL2_INCLUDE_DIR% (rem)
) else if not exist %SOUND_FILE% (rem)
) else (
    set DIRS_FOUND=True 
)

if %DIRS_FOUND%==False (
    echo One or more given paths from .env don't exist
    exit /b 1
)

rd /s /q build
mkdir build
cd build

cmake ..
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

cmake --build . --config %CONFIG%
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

cd ..

xcopy /Y "%SOUND_FILE%" ".\build\%CONFIG%\"
xcopy /Y "%SDL2_LIB_DIR%\SDL2.dll" ".\build\%CONFIG%\"

echo Copied files to build directory

endlocal