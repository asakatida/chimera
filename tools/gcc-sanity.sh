#!/usr/bin/env bash

set -ex -o pipefail

tools/cmake.sh build/gcc-make

cd build/gcc-make
make "-j$(nproc)"
