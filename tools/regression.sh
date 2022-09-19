#!/usr/bin/env bash

set -e -o pipefail

case $# in
  0 )
    echo "Usage: $0 <fuzzer> <corpus-dir>" >&2
    exit 1
    ;;
  2 )
    find "$2" -type f -print0 | \
      xargs --no-run-if-empty --null -- \
      "$0" "$1"
    ;;
  * )
    "$@" >>"/tmp/regression-$(basename "$1").logs" 2>&1
    ;;
esac
