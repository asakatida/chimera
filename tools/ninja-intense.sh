#!/usr/bin/env sh

set -ex

cd "$(git rev-parse --show-toplevel || true)"

tools/cmake.sh "$1"

python tools/ninja.py "$1"
ctest --build-and-test . "${build_root}" --build-generator Ninja --build-noclean
python tools/ninja.py "$1" check-stat
python tools/ninja.py "$1" corpus
python tools/ninja.py "$1" spec
