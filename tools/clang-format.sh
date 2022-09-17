#!/bin/bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

find . '(' -name '*.cpp' -or -name '*.h' -or -name '*.hpp' ')' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --null -n1 -P3 -- \
  clang-format -style=file -i "$@"
