#!/bin/sh
set -e
lcov --directory . --base-directory build --gcov-tool llvm-gcov.sh --capture -o build/cov.info
lcov --remove build/cov.info -o build/cov_nosys.info '/usr/include/*'
lcov --list build/cov_nosys.info
genhtml build/cov_nosys.info -o build/lcov
