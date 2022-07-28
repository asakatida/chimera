#!/bin/bash

set -ex -o pipefail

build="$1"
shift
root="$(git rev-parse --show-toplevel)"

find "${root}" -name '*.cpp' -print0 | \
  "${root}/tools/g-ls-tree.sh" | \
  xargs -0 -- clang-tidy -p="${build}" -quiet -fix -fix-errors -fix-notes "$@" \
  >"${build}/clang-tidy.log"
