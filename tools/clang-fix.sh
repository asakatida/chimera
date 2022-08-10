#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
build="${root}/build/clang-make"
scripts="${root}/tools"

"${scripts}/cmake.sh" "${build}"

make --directory "${build}" --jobs 3
"${scripts}/clang-format.sh"
make --directory "${build}" --jobs 3
"${scripts}/clang-tidy.sh" "${build}"
