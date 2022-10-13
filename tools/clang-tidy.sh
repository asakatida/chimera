#!/bin/bash

set -ex -o pipefail

if [[ "$(uname || true)" = Darwin ]]; then
  exit
fi

build="$1"
shift
cd "$(git rev-parse --show-toplevel || true)"

tools/g-ls-tree.sh -name '*.cpp' | \
  tools/xargs.sh -- \
  clang-tidy -p="${build}" -quiet -fix -fix-errors -fix-notes "$@" \
  >"${build}/clang-tidy.log"
