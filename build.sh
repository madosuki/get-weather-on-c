#!/bin/sh

echo "build start"

cd ./lib/http_connector
make

cd ../../
make

echo "done! (^ω^)⊃🍺"
