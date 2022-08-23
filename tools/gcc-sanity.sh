#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

"${root}/tools/cmake.sh" "${root}/build/gcc-ninja"

ninja -C "${root}/build/gcc-ninja" "$@"
