#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"
build_root="${root}/build"
debug_root="${build_root}/debug"

rm -rf "${debug_root}"

"${scripts}/boot.sh"

ninja -C "${debug_root}"
ninja -C "${debug_root}" check
ninja -C "${debug_root}" check_rand
