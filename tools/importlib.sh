#!/bin/bash

set -ex -o pipefail

build="$1"
shift
cd "$(git rev-parse --show-toplevel || true)"
output=library/virtual_machine/importlib.cpp

"${build}/builtins" "$@" < stdlib/_builtins.py | \
  clang-format -style=file >"${output}"

clang-tidy \
  -p="${build}" -quiet -fix -fix-errors -fix-notes \
  "${output}"
