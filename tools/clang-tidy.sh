#!/bin/bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

find "${root}" -name '*.cpp' -print0 | \
  "${root}/tools/g-ls-tree.sh" | \
  xargs -0 -- clang-tidy -p="$1" -quiet -fix -fix-errors -fix-notes \
  >"$1/clang-tidy.log"
