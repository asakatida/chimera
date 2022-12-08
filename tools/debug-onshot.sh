#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"
build_root=build/debug

rm -rf "${build_root}"

tools/boot.sh

python tools/ninja.py "${build_root}" check
python tools/ninja.py "${build_root}" check-rand
