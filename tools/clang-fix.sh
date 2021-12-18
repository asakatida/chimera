#!/usr/bin/env bash

set -ex -o pipefail

source="$(pwd)"

tools/cmake.sh build/clang-make

pushd build/clang-make
make "-j$(nproc)"

find "${source}/"{library,source,tools,unit_tests} '(' -name '*.cpp' -or -name '*.h' -or -name '*.hpp' ')' -print0 | xargs -0 -n1 -P8 -- clang-format -style=file -i
find "${source}/"{library,source,tools,unit_tests} -name '*.cpp' -print0 | xargs -0 -- clang-tidy "-p=${source}" -fix -fix-errors -format-style=file -header-filter='\.\*'
