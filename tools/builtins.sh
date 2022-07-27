#!/bin/bash

set -ex -o pipefail

build="$1"
shift
root="$(git rev-parse --show-toplevel)"
output="${root}/library/virtual_machine/builtins.cpp"

"${build}/builtins" "$@" < "${root}/stdlib/_builtins.py" | \
clang-format -style=file >"${output}"

clang-tidy \
  -p="${build}" -quiet -fix -fix-errors -fix-notes \
  "${output}"
