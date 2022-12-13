#!/bin/bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

python tools/g_ls_tree.py cpp h hpp | \
  tools/xargs.sh -n1 -P3 -- \
  clang-format -style=file -i "$@"
