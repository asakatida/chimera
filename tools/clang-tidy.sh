#!/bin/bash

set -ex -o pipefail

if [[ "$(uname || true)" = Darwin ]]; then
  exit
fi

build="$1"
shift
cd "$(git rev-parse --show-toplevel || true)"

find . -name '*.cpp' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --no-run-if-empty --null -- \
  clang-tidy -p="${build}" -quiet -fix -fix-errors -fix-notes "$@" \
  >"${build}/clang-tidy.log"
