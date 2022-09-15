#!/usr/bin/env bash

set -ex -o pipefail

find "$2" -type f -print0 | \
  xargs --no-run-if-empty --null -- \
  "./$1"
