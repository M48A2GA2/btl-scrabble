@echo off
echo Building Scrabble Game...

REM Biên dịch với g++ (yêu cầu MinGW hoặc MSYS2)
g++ -std=c++17 -Wall -g ^
    src\*.cpp ^
    -o scrabble.exe ^
    -lSDL2 -lSDL2_ttf ^
    -Llib ^
    -Iinclude

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Run scrabble.exe to play.
) else (
    echo Build failed! Make sure you have:
    echo - MinGW or MSYS2 with g++
    echo - SDL2 and SDL2_ttf libraries
    echo - Proper library paths configured
)

pause
