#!/usr/bin/env bash

set -ex -o pipefail

tools/cmake.sh "$1" -GNinja

pushd "$1"

if [[ "${CXX}" = clang* ]]; then
  ninja
fi

ninja bnf2abnf
ninja builtins
ninja fuzz
ninja fuzz2
ninja importlib
ninja modules_tools
ninja unit_test

ninja libchimera
ninja chimera

ninja spec
ninja corpus
ninja corpus2
