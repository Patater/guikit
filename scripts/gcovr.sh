#!/bin/sh
set -e
mkdir -p build/gcovr
(cd build && gcovr -s -r .. --gcov-executable llvm-gcov.sh --html --html-details -o gcovr/index.html)
