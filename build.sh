#!/bin/sh

git submodule init && git submodule update

if [ ! -d ./libopenweathermap_client ]; then
  echo "Error: git submodule with libopenweathermap_client"
  return
fi

cd ./libopenweathermap_client

git checkout main

git submodule init && git submodule update

if [ ! -d ./libhttp_connector ]; then
  echo "Error: git submodule with libhttp_connector"
  return
fi

cd ../

if [ ! -d ./build_linux ]; then
    mkdir -p build_linux
fi

cd build_linux

cmake ..

echo "build start"

cmake --build .

echo "done! (^ω^)⊃🍺"
