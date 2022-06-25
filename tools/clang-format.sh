#!/bin/bash

set -ex -o pipefail

find . '(' -name '*.cpp' -or -name '*.h' -or -name '*.hpp' ')' -print0 | \
  xargs -0 -- git ls-tree --full-tree --name-only -z HEAD -- | \
  xargs -0 -n1 -P3 \
  clang-format -style=file -i
