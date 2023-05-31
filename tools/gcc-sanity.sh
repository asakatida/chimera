#!/usr/bin/env sh

set -ex

cd "$(git rev-parse --show-toplevel || true)"

tools/cmake.sh "${PWD}/build/gcc-ninja"

python tools/ninja.py build/gcc-ninja "$@"
