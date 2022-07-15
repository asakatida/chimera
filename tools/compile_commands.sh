#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"

export CPPFLAGS="${CPPFLAGS} \
  -I ${root}/include \
  -I ${root}/library \
  -I ${root}/unit_tests \
  -isystem ${root}/external/Catch/src \
  -isystem ${root}/external/GSL/include \
  -isystem ${root}/external/metal/include \
  -isystem ${root}/external/operators/include \
  -isystem ${root}/external/PEGTL/include"

export CXXFLAGS="${CXXFLAGS} \
  -std=c++20 \
  -Werror \
  -Wno-c++11-extensions \
  -Wno-c++17-extensions \
  -Wno-c++98-compat-pedantic \
  -Wno-date-time \
  -Wno-padded \
  -fbracket-depth=1024"

# shellcheck disable=SC2016
find "${root}" -name '*.cpp' -print0 | \
  xargs -0 -- git ls-tree --full-tree --name-only -z HEAD -- | \
  xargs -0 -I'{}' -- \
  jq \
  --arg cxx "${CXX:-clang}" \
  --arg cppflags "${CPPFLAGS}" \
  --arg cxxflags "${CXXFLAGS}" \
  --arg root "${root}" \
  --arg file '{}' \
  -n '
    {
      "root": $root,
      "command": ([$cxx, $cppflags, $cxxflags, "-c", $file] | join(" ")),
      "file": $file
    }
  ' | \
  jq -s
