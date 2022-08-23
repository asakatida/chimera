#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"

"${scripts}/cmake.sh" "$1"

pushd "$1"

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
