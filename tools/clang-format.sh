#!/bin/bash

git diff --stat @\{2.days.ago\} --name-only | \
grep -e '\.h' -e '\.hpp' -e '\.cpp' | \
sort | uniq | \
xargs -n1 -P3 \
clang-format -style=file -i
