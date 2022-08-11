#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"

scan-build "${scripts}/debug-onshot.sh"
scan-build --use-cc clang --use-c++ clang++ "${scripts}/debug-onshot.sh"
