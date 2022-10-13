#!/usr/bin/env bash

set -e -o pipefail

ninja -C "$1" -j1 chimera-grammar
ninja -C "$1" -j2 chimera unit-test
ninja -C "$1"
ninja -C "$@"
