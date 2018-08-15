#!/bin/bash

git diff --stat @\{2.days.ago\} --name-only | \
grep -e '\.cpp' | \
sort | uniq | \
xargs -n1 -P1 \
clang-tidy -p="$1" \
-quiet -fix -fix-errors -format-style=file -config='' -header-filter=.\*
