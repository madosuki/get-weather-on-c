#!/bin/sh

echo "build start"

cd ./src/lib/http_connector
make

cd ../
make

echo "done! (^ω^)⊃🍺"
