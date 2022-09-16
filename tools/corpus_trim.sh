#!/usr/bin/env bash

set -e -o pipefail

case "$0" in
  /* )
    self_bin="$0"
    ;;
  * )
    self_bin="$(pwd)/$0"
    ;;
esac

case $# in
  0 )
    set -x
    cd "$(git rev-parse --show-toplevel || true)"
    if [[ -z "$(find . -name fuzz-vm -perm -0110 -type f || true)" ]]; then
      exit 1
    fi
    mkdir -p unit_tests/fuzz/{corpus,crashes}
    parallel="$(nproc)"
    find unit_tests/fuzz/{corpus,crashes} -type f -print0 | \
      xargs --no-run-if-empty --null -n1 -P"${parallel}" -- \
      "${self_bin}"
    find . \
      '(' -name 'crash-*' -or -name 'leak-*' -or -name 'timeout-*' ')' \
      -type f -exec mv '{}' unit_tests/fuzz/crashes ';'
    mv unit_tests/fuzz/corpus unit_tests/fuzz/corpus_original
    mkdir unit_tests/fuzz/corpus
    "${self_bin}" \
      -detect_leaks=0 \
      -merge=1 \
      -reduce_inputs=1 \
      -shrink=1 \
      -use_value_profile=1 \
      unit_tests/fuzz/corpus \
      unit_tests/fuzz/corpus_original
    rm -rf unit_tests/fuzz/corpus_original
    python3 tools/corpus_trim.py
    ;;
  1 )
    case "$1" in
      */crashes/* )
        if "$0" -detect_leaks=0 -use_value_profile=1 "$1"; then
          mv "$1" unit_tests/fuzz/corpus
        fi
        ;;
      */corpus/* )
        if ! "$0" -detect_leaks=0 -use_value_profile=1 "$1"; then
          mv "$1" unit_tests/fuzz/crashes
        fi
        ;;
      * )
        echo "usage: $0" >&2
        exit 1
        ;;
    esac
    ;;
  * )
    find . -name 'fuzz-*' -perm -0110 -type f -print0 | \
      xargs --no-run-if-empty --null --replace='{}' -- \
      env '{}' "$@" >/dev/null 2>&1
    ;;
esac
