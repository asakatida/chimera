#!/usr/bin/env sh

set -ex

cd "$(git rev-parse --show-toplevel || true)"

if [ -d env/bin ]; then
  export PATH="${PWD}/env/bin:${PATH}"
elif [ -d /opt/virtualenv/bin ]; then
  export PATH="/opt/virtualenv/bin:${PATH}"
else
  python3 tools/venv.py
  export PATH="${PWD}/env/bin:${PATH}"
fi

python tools/g_ls_tree.py yml yaml | \
  tools/xargs.sh -- \
  yamllint "$@"
