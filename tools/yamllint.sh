#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

python_bin="$(command -v python3)"
[[ -d env/bin ]] || tools/venv.sh "${python_bin}"
export PATH="${PWD}/env/bin:${PATH}"

find . -name '*.yml' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --no-run-if-empty --null -- \
  yamllint "$@"
