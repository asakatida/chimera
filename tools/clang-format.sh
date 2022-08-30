#!/bin/bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

find . '(' -name '*.cpp' -or -name '*.h' -or -name '*.hpp' ')' -print0 | \
  tools/g-ls-tree.sh | \
  xargs -0 -n1 -P3 -- \
  clang-format -style=file -i "$@"
