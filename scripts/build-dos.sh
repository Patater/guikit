#!/bin/sh
set -e
cmake -S . -B dosbuild -DCMAKE_TOOLCHAIN_FILE=cmake/DJGPP.cmake -DENABLE_WARNINGS=1 -DCMAKE_BUILD_TYPE=Release -GNinja
cmake --build dosbuild
#test-runner -v -j 7 -p dosbuild/tests
