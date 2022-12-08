#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

tools/cmake.sh "$1"

python tools/ninja.py "$1" check-rand
python tools/ninja.py "$1" check-stat
python tools/ninja.py "$1" corpus
python tools/ninja.py "$1" spec
