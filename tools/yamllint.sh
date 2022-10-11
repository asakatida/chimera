#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

if [[ -d env/bin ]]; then
  export PATH="${PWD}/env/bin:${PATH}"
elif [[ -d /opt/virtualenv/bin ]]; then
  export PATH="/opt/virtualenv/bin:${PATH}"
else
  tools/venv.sh python3
  export PATH="${PWD}/env/bin:${PATH}"
fi

find . -name '*.yml' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --no-run-if-empty --null -- \
  yamllint "$@"
