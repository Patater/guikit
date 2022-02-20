#!/bin/sh
set -e
cmake -S . -B build -DENABLE_SANITIZERS=1 -DENABLE_COVERAGE=1 -DENABLE_WARNINGS=1 -DENABLE_IWYU=1 -DCMAKE_BUILD_TYPE=Debug -GNinja
cmake --build build
cmake --build build --target test
#test-runner -v -j 7 -p build/tests
