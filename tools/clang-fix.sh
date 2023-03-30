#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"
build=build/clang-ninja

tools/cmake.sh "${PWD}/${build}"

python tools/ninja.py "${build}"
python tools/clang_format.py
python tools/ninja.py "${build}"
python tools/clang_tidy.py "${build}"
