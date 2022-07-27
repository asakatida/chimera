#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
build="${root}/build/clang-make"
scripts="${root}/tools"

"${scripts}/cmake.sh" "${build}"

make -b "${build}" "-j$(nproc)"
"${scripts}/clang-format.sh"
make -b "${build}" "-j$(nproc)"
"${scripts}/clang-tidy.sh" "${build}"
