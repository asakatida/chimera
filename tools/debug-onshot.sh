#!/usr/bin/env sh

set -ex

cd "$(git rev-parse --show-toplevel || true)"
build_root=build/debug

rm -rf "${build_root}"

tools/boot.sh

python tools/ninja.py "${build_root}" unit-test
python tools/ninja.py "${build_root}" test
