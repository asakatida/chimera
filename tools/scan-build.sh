#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"

scan-build-15 "${scripts}/debug-onshot.sh"
scan-build-15 --use-cc clang-15 --use-c++ clang++-15 "${scripts}/debug-onshot.sh"
