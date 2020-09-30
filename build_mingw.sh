#!/bin/sh

if [ ! -d ./build_mingw ]; then
    mkdir -p build_mingw
fi

cd build_mingw

cmake ..

echo "build start"

cmake --build .

echo "done! (^ω^)⊃🍺"
