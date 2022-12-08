#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"
build=build/clang-ninja

tools/cmake.sh "${PWD}/${build}"

python tools/ninja.py "${build}"
tools/clang-format.sh
python tools/ninja.py "${build}"
tools/clang-tidy.sh "${build}"
