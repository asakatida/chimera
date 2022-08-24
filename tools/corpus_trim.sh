#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

mv "${root}/unit_tests/fuzz/corpus" "${root}/unit_tests/fuzz/corpus_original"
mkdir "${root}/unit_tests/fuzz/corpus"

find "${root}/build/" -name 'fuzz-*' -perm -0110 -type f \
  -exec '{}' -merge=1 \
  "${root}/unit_tests/fuzz/corpus" \
  "${root}/unit_tests/fuzz/corpus_original" ';'

rm -rf "${root}/unit_tests/fuzz/corpus_original"

python3 "${root}/tools/corpus_trim.py"
