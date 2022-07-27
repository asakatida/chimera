#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

"${root}/tools/cmake.sh" "${root}/build/gcc-make"

make -b "${root}/build/gcc-make" "-j$(nproc)" "$@"
