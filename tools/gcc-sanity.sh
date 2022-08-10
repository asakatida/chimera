#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

"${root}/tools/cmake.sh" "${root}/build/gcc-make"

make --directory "${root}/build/gcc-make" --jobs 3 "$@"
