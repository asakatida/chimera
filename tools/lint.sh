#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

tools/shellcheck.sh

export PATH="${PWD}/env/bin:${PATH}"

cmakelint

find . -name '*.py' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --null -- \
  isort --python-version auto
find . -name '*.py' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --null -- \
  black --preview --target-version py311
find . -name '*.py' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --null -- \
  pylama
find . -not -path "./stdlib/*" -name '*.py' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --null -- \
  mypy

python3 tools/generate_utf8_id_continue.py
python3 tools/generate_utf8_id_start.py
python3 tools/generate_utf8_space.py

git ls-tree --full-tree --name-only -r -z HEAD | \
  xargs --null -- \
  python3 tools/trim.py

if [[ -z "${PREBUILD_CHECK}" ]]; then
  tools/clang-format.sh
  tools/clang-tidy.sh build/debug
fi
