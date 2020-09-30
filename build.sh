#!/bin/sh

if [ ! -d ./build_linux ]; then
    mkdir -p build_linux
fi

cd build_linux

cmake ..

echo "build start"

cmake --build .

echo "done! (^ω^)⊃🍺"
