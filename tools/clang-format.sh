#!/bin/bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

find "${root}" '(' -name '*.cpp' -or -name '*.h' -or -name '*.hpp' ')' -print0 | \
  "${root}/tools/g-ls-tree.sh" | \
  xargs -0 -n1 -P3 \
  clang-format -style=file -i
