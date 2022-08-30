#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"
build=build/clang-ninja

tools/cmake.sh "${PWD}/${build}"

ninja -C "${build}"
tools/clang-format.sh
ninja -C "${build}"
tools/clang-tidy.sh "${build}"
