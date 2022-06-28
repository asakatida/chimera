#!/usr/bin/env bash

set -ex -o pipefail

tools/cmake.sh build/clang-make

pushd build/clang-make
make "-j$(nproc)"
popd
tools/clang-format.sh
pushd build/clang-make
make "-j$(nproc)"
tools/clang-tidy.sh
popd
