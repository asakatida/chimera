#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

if [[ -z "$(find . -name fuzz-vm -perm -0110 -type f || true)" ]]; then
  exit
fi

mkdir -p unit_tests/fuzz/crashes
find build \
  '(' -name 'crash-*' -or -name 'leak-*' -or -name 'timeout-*' ')' \
  -type f -exec mv {} unit_tests/fuzz/crashes ';'
pushd unit_tests/fuzz/crashes
# shellcheck disable=SC2016
find . -name '*-*' -type f -print0 | \
  xargs --no-run-if-empty --null -- \
  bash -c 'mv "$1" "./${1/.*-/}"' --
popd

mv unit_tests/fuzz/corpus unit_tests/fuzz/corpus_original
mkdir unit_tests/fuzz/corpus

find . -name 'fuzz-*' -perm -0110 -type f -print0 | \
  xargs --no-run-if-empty --null --replace='{}' --verbose -- env \
  '{}' -merge=1 \
  -reduce_inputs=1 \
  -shrink=1 \
  -use_value_profile=1 \
  unit_tests/fuzz/corpus \
  unit_tests/fuzz/corpus_original \
  || true

rm -rf unit_tests/fuzz/corpus_original

python3 tools/corpus_trim.py
