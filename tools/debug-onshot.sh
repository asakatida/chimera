#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"
build_root=build/debug

rm -rf "${build_root}"

tools/boot.sh

ninja -C "${build_root}"
ninja -C "${build_root}" check
ninja -C "${build_root}" check_rand
