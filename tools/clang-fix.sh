#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
build="${root}/build/clang-ninja"
scripts="${root}/tools"

"${scripts}/cmake.sh" "${build}"

ninja -C "${build}"
"${scripts}/clang-format.sh"
ninja -C "${build}"
"${scripts}/clang-tidy.sh" "${build}"
