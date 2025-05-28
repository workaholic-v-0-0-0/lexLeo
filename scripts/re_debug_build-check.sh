#!/bin/bash

rm -r build/
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build
cmake --build build --target check
