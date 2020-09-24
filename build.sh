#!/bin/sh


# echo "build start"

# cd ./src/lib/http_connector
# make

# cd ../
# make

if [ -d ./build ]; then
    mkdir -p build
fi

cd build

cmake ..

cmake --build .

echo "done! (^ω^)⊃🍺"
