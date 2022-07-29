#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"
build_root="${root}/build"
debug_root="${build_root}/debug"

"${scripts}/shellcheck.sh"

export PATH="${root}/env/bin:${PATH}"

cmakelint

find "${root}" -name '*.py' -print0 | "${scripts}/g-ls-tree.sh" | xargs -0 -- isort --python-version auto
find "${root}" -name '*.py' -print0 | "${scripts}/g-ls-tree.sh" | xargs -0 -- black --preview --target-version py310
find "${root}" -name '*.py' -print0 | "${scripts}/g-ls-tree.sh" | xargs -0 -- pylama
find "${root}" -not -path "${root}/stdlib/*" -name '*.py' -print0 | "${scripts}/g-ls-tree.sh" | xargs -0 -- mypy

python3 "${scripts}/generate_utf8_id_continue.py"
python3 "${scripts}/generate_utf8_id_start.py"
python3 "${scripts}/generate_utf8_space.py"

git ls-tree --full-tree --name-only -r -z HEAD | xargs -0 -- python3 "${scripts}/trim.py"

"${scripts}/clang-format.sh"
"${scripts}/clang-tidy.sh" "${debug_root}"
