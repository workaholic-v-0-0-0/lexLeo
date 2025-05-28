#!/bin/bash

for i in {1..50} ; do printf "\n" ; done
rm -r build/
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build
cmake --build build --target open_doc
