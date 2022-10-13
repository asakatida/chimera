#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

tools/g-ls-tree.sh -name '*.sh' | \
  tools/xargs.sh -- \
  shellcheck --enable=all --severity=style "$@"
