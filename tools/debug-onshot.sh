#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"
build_root=build/debug

rm -rf "${build_root}"

tools/boot.sh

tools/ninja.sh "${build_root}"
tools/ninja.sh "${build_root}" check
tools/ninja.sh "${build_root}" check-rand
