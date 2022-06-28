#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

find "${root}" -name '*.sh' -print0 | \
    xargs -0 -- git ls-tree --full-tree --name-only -z HEAD -- | \
    xargs -0 -- shellcheck --severity=info --enable=all
