#!/bin/bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

find "${root}" -name '*.cpp' -print0 | \
  xargs -0 -- git ls-tree --full-tree --name-only -z HEAD -- | \
  xargs -0 -- clang-tidy -p="${root}" -quiet -fix -fix-errors -fix-notes
