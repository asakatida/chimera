#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"

python_bin="$(command -v python3)"
[[ -d "${root}/env/bin" ]] || "${scripts}/venv.sh" "${python_bin}"
export PATH="${root}/env/bin:${PATH}"

find "${root}" -name '*.yml' -print0 | \
    "${scripts}/g-ls-tree.sh" | \
    xargs -0 -- yamllint "$@"
