#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

if [[ -z "$(find "${root}" -name fuzz-vm -perm -0110 -type f || true)" ]]; then
  exit
fi

find "${root}/build/" \
  '(' -name 'crash-*' -or -name 'leak-*' -or -name 'timeout-*' ')' \
  -type f -exec mv {} "${root}/unit_tests/fuzz/crashes" ';'
pushd "${root}/unit_tests/fuzz/crashes"
# shellcheck disable=SC2016
find . -name '*-*' -type f -print0 | \
  xargs --no-run-if-empty --null -- \
  bash -c 'mv "$1" "./${1/.*-/}"' --
popd

mv "${root}/unit_tests/fuzz/corpus" "${root}/unit_tests/fuzz/corpus_original"
mkdir "${root}/unit_tests/fuzz/corpus"

find "${root}" -name fuzz-vm -perm -0110 -type f -print0 | \
  xargs --no-run-if-empty --null --replace='{}' --verbose -- env \
  '{}' -merge=1 \
  -reduce_inputs=1 \
  -shrink=1 \
  -use_value_profile=1 \
  "${root}/unit_tests/fuzz/corpus" \
  "${root}/unit_tests/fuzz/corpus_original"

rm -rf "${root}/unit_tests/fuzz/corpus_original"

python3 "${root}/tools/corpus_trim.py"
