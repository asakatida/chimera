#!/bin/bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

tools/g-ls-tree.sh '(' -name '*.cpp' -or -name '*.h' -or -name '*.hpp' ')' | \
  tools/xargs.sh -n1 -P3 -- \
  clang-format -style=file -i "$@"
