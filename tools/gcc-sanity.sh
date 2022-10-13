#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

tools/cmake.sh "${PWD}/build/gcc-ninja"

tools/ninja.sh build/gcc-ninja "$@"
