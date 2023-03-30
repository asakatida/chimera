#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

case "$(uname)" in
  Darwin )
    export PATH="${PWD}/env/bin:${PATH}"
    ;;
  Linux )
    if command -v apt; then
      echo 'Apt found, expecting Dockerfile is used'
      export PATH="/opt/virtualenv/bin:${PATH}"
    else
      echo 'No apt found, failed installation'
      exit 1
    fi
    ;;
  * )
    uname
    echo 'unknown os, failed installation'
    exit 1
    ;;
esac

cmakelint

python tools/g_ls_tree.py py | \
  tools/xargs.sh -- \
  isort --python-version auto
python tools/g_ls_tree.py py | \
  tools/xargs.sh -- \
  black --preview --target-version py311
python tools/g_ls_tree.py py | \
  tools/xargs.sh -- \
  pylama
python tools/g_ls_tree.py py --exclude 'stdlib/|.*/stdlib/' | \
  tools/xargs.sh -- \
  mypy

python3 tools/generate_utf8_id_continue.py
python3 tools/generate_utf8_id_start.py
python3 tools/generate_utf8_space.py
git ls-tree --full-tree --name-only -r -z HEAD | \
  tools/xargs.sh -- \
  python3 tools/trim.py
python tools/clang_format.py
python tools/clang_tidy.py build/debug
