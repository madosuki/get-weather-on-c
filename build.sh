#!/bin/sh

if [ ! -d ./build ]; then
    mkdir -p build
fi

cd build

cmake ..

echo "build start"

cmake --build .

echo "done! (^ω^)⊃🍺"
