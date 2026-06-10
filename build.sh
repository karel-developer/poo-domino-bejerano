#!/bin/bash
# Build script for Linux
set -e

# Check nlohmann/json
if [ ! -f "libs/nlohmann/json.hpp" ]; then
    echo "ERROR: libs/nlohmann/json.hpp not found."
    echo "Download from: https://github.com/nlohmann/json/releases"
    exit 1
fi

# Detect SFML via pkg-config or assume system-installed
SFML_FLAGS=$(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system 2>/dev/null) || {
    echo "SFML not found via pkg-config. Trying default paths..."
    SFML_FLAGS="-lsfml-graphics -lsfml-window -lsfml-system"
}

mkdir -p assets datos

echo "Compiling..."
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic \
    src/main.cpp \
    -I src -I libs \
    $SFML_FLAGS \
    -o domino

echo "Build OK: ./domino"
