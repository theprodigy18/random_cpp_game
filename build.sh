#!/bin/bash

OS_NAME="$(uname -s)"

SOURCES="src/main.cpp $(find src/utils -name '*.cpp') $(find src/shared -name '*.cpp')"

INCLUDES="-Iinclude -Ithird_party"
WARNINGS="-Wno-writable-strings -Wno-format-security -Wno-write-strings"

if [[ "$OS_NAME" == MINGW* || "$OS_NAME" == MSYS* || "$OS_NAME" == CYGWIN* ]]; then
    echo "Running on Windows (via Git Bash, MSYS, or Cygwin)"
    LIBS="-luser32 -lgdi32 -lopengl32"
    SOURCES="${SOURCES} src/platform/window_win32.cpp src/renderer/opengl_win32.cpp"
    OUTPUT="build/x64-Debug/game.exe"
    COMPILER="clang++"
elif [[ "$OS_NAME" == Linux ]]; then
    echo "Running on Linux"
    LIBS="-lX11 -lGL -lGLX"
    SOURCES="${SOURCES} src/platform/window_linux.cpp src/renderer/opengl_linux.cpp"
    OUTPUT="build/linux-Debug/game"
    COMPILER="g++"
else
    echo "Unknown OS: $OS_NAME"
    exit 1
fi


mkdir -p "${OUTPUT%/*}"

$COMPILER -g $SOURCES -std=c++17 -o$OUTPUT $LIBS $INCLUDES $WARNINGS 
