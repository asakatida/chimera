#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

tools/cmake.sh "$1"

tools/ninja.sh "$1"

tools/ninja.sh "$1" check-rand
tools/ninja.sh "$1" check-stat
tools/ninja.sh "$1" corpus
tools/ninja.sh "$1" spec
