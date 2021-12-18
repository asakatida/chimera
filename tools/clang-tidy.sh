#!/bin/bash

set -ex -o pipefail

git diff --stat '@{2.days.ago}' --name-only | \
  grep -F -e '.h' -e '.hpp' -e '.cpp' | \
  sort -u | \
  xargs -n1 -P1 \
  clang-tidy -p="$1" \
  -quiet -fix -fix-errors -format-style=file -config='' -header-filter='.\*'
