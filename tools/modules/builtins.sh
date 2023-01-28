#!/bin/bash

set -ex -o pipefail

build="$1"
shift
cd "$(git rev-parse --show-toplevel || true)"
output=stdlib/builtins/builtins.cpp

"${build}/builtins" "$@" < stdlib/builtins_.py | \
  clang-format -style=file >"${output}"
