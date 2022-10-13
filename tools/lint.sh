#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

tools/shellcheck.sh

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

tools/g-ls-tree.sh -name '*.py' | \
  tools/xargs.sh -- \
  isort --python-version auto
tools/g-ls-tree.sh -name '*.py' | \
  tools/xargs.sh -- \
  black --preview --target-version py311
tools/g-ls-tree.sh -name '*.py' | \
  tools/xargs.sh -- \
  pylama
tools/g-ls-tree.sh -not -path "./stdlib/*" -name '*.py' | \
  tools/xargs.sh -- \
  mypy

if [[ -z "${PREBUILD_CHECK}" ]]; then
  python3 tools/generate_utf8_id_continue.py
  python3 tools/generate_utf8_id_start.py
  python3 tools/generate_utf8_space.py
  git ls-tree --full-tree --name-only -r -z HEAD | \
    tools/xargs.sh -- \
    python3 tools/trim.py
  tools/clang-format.sh
  tools/clang-tidy.sh build/debug
fi
