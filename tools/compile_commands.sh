#!/usr/bin/env bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

export CPPFLAGS="${CPPFLAGS} \
  -I ${PWD}/include \
  -I ${PWD}/library \
  -I ${PWD}/unit_tests \
  -isystem ${PWD}/external/Catch/src \
  -isystem ${PWD}/external/GSL/include \
  -isystem ${PWD}/external/metal/include \
  -isystem ${PWD}/external/operators/include \
  -isystem ${PWD}/external/PEGTL/include \
  -DCHIMERA_PATH=${PWD}/stdlib"

export CXXFLAGS="${CXXFLAGS} \
  -std=c++20 \
  -O0 \
  -Wall -Wpedantic -Werror \
  -Wno-c++11-extensions \
  -Wno-c++17-extensions \
  -Wno-c++98-compat-pedantic \
  -Wno-date-time \
  -Wno-padded"

# shellcheck disable=SC2016
find . -name '*.cpp' -print0 | \
  tools/g-ls-tree.sh | \
  xargs --no-run-if-empty --null --replace'{}' -- \
  jq \
  --arg cxx "${CXX:-clang++}" \
  --arg cppflags "${CPPFLAGS}" \
  --arg cxxflags "${CXXFLAGS}" \
  --arg root "${PWD}" \
  --arg file '{}' \
  -n '
    {
      "directory": ($root + "/build/debug"),
      "command": ([$cxx, $cppflags, $cxxflags, "-o", "CMakeFiles/chimera_core.dir/" + $file + ".o", "-c", $file] | join(" ")),
      "file": $file
    }
  ' | \
  jq -s
