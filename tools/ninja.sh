#!/usr/bin/env bash

set -e -o pipefail

build="$1"
shift

ninja -C "${build}" -j1 chimera-grammar
ninja -C "${build}" -j3 libchimera
ninja -C "${build}" -j1 fuzzers
ninja -C "${build}" Catch2
ninja -C "${build}" -j2 chimera unit-test
ninja -C "${build}"
ninja -C "${build}" -j3 "$@"
