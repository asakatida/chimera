#!/usr/bin/env bash

set -ex -o pipefail

source="$(pwd)"

tools/cmake.sh build/clang-make

pushd build/clang-make
make "-j$(nproc)"
popd
tools/clang-format.sh
pushd build/clang-make
make "-j$(nproc)"
find "${source}/"{library,source,tools,unit_tests} -name '*.cpp' -print0 | xargs -0 -- clang-tidy "-p=${source}" -fix -fix-errors -format-style=file -header-filter='\.\*'
popd
