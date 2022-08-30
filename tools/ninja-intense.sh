#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

tools/cmake.sh "$1"

cd "$1"

if [[ "${CXX:-}" = clang* ]]; then
  ninja
fi

ninja bnf2abnf
ninja builtins
ninja fuzz-number
ninja fuzz
ninja importlib
ninja modules_tools
ninja unit_test

ninja libchimera
ninja chimera

ninja corpus-number
ninja corpus
ninja spec
