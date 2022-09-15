#!/usr/bin/env bash

set -ex -o pipefail

if [[ $# -eq 2 ]] && [[ "$1" == corpus ]]; then
  if "$0" "$2"; then
    mv "$1" unit_tests/fuzz/corpus
  fi
  exit
elif [[ $# -eq 2 ]] && [[ "$1" == crashes ]]; then
  if ! "$0" "$2"; then
    mv "$1" unit_tests/fuzz/crashes
  fi
  exit
fi

case $# in
  0 )
    cd "$(git rev-parse --show-toplevel || true)"
    if [[ -z "$(find . -name fuzz-vm -perm -0110 -type f || true)" ]]; then
      exit 1
    fi
    mkdir -p unit_tests/fuzz/crashes
    find unit_tests/fuzz/crashes -type f -exec "$0" corpus '{}' ';' || true
    find unit_tests/fuzz/corpus -type f -exec "$0" crashes '{}' ';' || true
    find build \
      '(' -name 'crash-*' -or -name 'leak-*' -or -name 'timeout-*' ')' \
      -type f -exec mv '{}' unit_tests/fuzz/crashes ';'
    mv unit_tests/fuzz/corpus unit_tests/fuzz/corpus_original
    mkdir unit_tests/fuzz/corpus
    find . -name 'fuzz-*' -perm -0110 -type f -print0 | \
      xargs --no-run-if-empty --null --replace='{}' -- \
      env '{}' \
      -merge=1 \
      -reduce_inputs=1 \
      -shrink=1 \
      -use_value_profile=1 \
      unit_tests/fuzz/corpus \
      unit_tests/fuzz/corpus_original >/dev/null 2>&1
    rm -rf unit_tests/fuzz/corpus_original
    python3 tools/corpus_trim.py
    ;;
  1 )
    find . -name 'fuzz-*' -perm -0110 -type f -print0 | \
      xargs --no-run-if-empty --null --replace='{}' -- \
      env '{}' "$1" >/dev/null 2>&1
    ;;
  * )
    echo "usage: $0" >&2
    exit 1
    ;;
esac
