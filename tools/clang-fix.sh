#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"
build=build/clang-ninja

tools/cmake.sh "${PWD}/${build}"

tools/ninja.sh "${build}"
tools/clang-format.sh
tools/ninja.sh "${build}"
tools/clang-tidy.sh "${build}"
