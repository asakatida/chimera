#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"

find "${root}" -name '*.sh' -print0 | \
    "${scripts}/g-ls-tree.sh" | \
    xargs -0 -- shellcheck --severity=info --enable=all "$@"
