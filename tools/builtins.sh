#!/bin/bash

set -ex -o pipefail

output=library/virtual_machine/builtins.cpp

"$1/builtins" < stdlib/_builtins.py | \
clang-format -style=file >"${output}"

clang-tidy -p="$1" \
  -quiet -fix -fix-errors -format-style=file -config='' -header-filter=.\* \
  "${output}"
