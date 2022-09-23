#!/usr/bin/env bash

set -e -o pipefail

case $# in
  2 )
    find "$2" -type f -print0 | \
      xargs --no-run-if-empty --null -- \
      "$1" -detect_leaks=0 -use_value_profile=1 -max_total_time=500 \
      >"/tmp/regression-$(basename "$1").logs" 2>&1
    ;;
  * )
    echo "Usage: $0 <fuzzer> <corpus-dir>" >&2
    exit 1
    ;;
esac
