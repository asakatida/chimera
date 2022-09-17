#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

find . -name '*.sh' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --null -- \
  shellcheck --enable=all --severity=style --shell=bash --exclude=SC2096 "$@"
