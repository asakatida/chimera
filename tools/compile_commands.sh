#!/usr/bin/env bash

set -ex -o pipefail

directory="$(pwd)"

export CPPFLAGS="${CPPFLAGS} \
  -I ${directory}/include \
  -I ${directory}/library \
  -I ${directory}/unit_tests \
  -isystem ${directory}/external/Catch/single_include \
  -isystem ${directory}/external/GSL/include \
  -isystem ${directory}/external/metal/include \
  -isystem ${directory}/external/operators/include \
  -isystem ${directory}/external/PEGTL/include"

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
find ./{library,source,tools,unit_tests} -name '*.cpp' -print0 | xargs -0 -I'{}' -- jq --arg cxx "${CXX:-clang}" --arg cppflags "${CPPFLAGS}" --arg cxxflags "${CXXFLAGS}" --arg directory "${directory}" --arg file '{}' -n '{"directory": $directory, "command": ([$cxx, $cppflags, $cxxflags, "-c", $file] | join(" ")), "file": $file}' | jq -s
