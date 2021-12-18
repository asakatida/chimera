#!/bin/bash

set -ex -o pipefail

git diff --stat '@{2.days.ago}' --name-only | \
  grep -F -e '.h' -e '.hpp' -e '.cpp' | \
  sort -u | \
  xargs -n1 -P3 \
  clang-format -style=file -i
