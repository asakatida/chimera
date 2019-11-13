#!/usr/bin/env sh
# docker run alpine cmake.alpine.sh

set -ex

cd "$(dirname "$0")"

mkdir -p build
cd build

export CXXFLAGS='-Weverything'
CXX="$(command -v clang++)"
export CXX

cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_SHARED_LIBS=OFF ..

ninja
ninja spec
ninja corpus

# export CPPFLAGS="${CPPFLAGS} \
#   -I ${SOURCE}/include \
#   -I ${SOURCE}/library \
#   -I ${SOURCE}/unit_tests \
#   -isystem ${SOURCE}/external/GSL/include \
#   -isystem ${SOURCE}/external/metal/include \
#   -isystem ${SOURCE}/external/PEGTL/include \
#   -isystem ${SOURCE}/external/Catch/single_include"
# export CXXFLAGS="${CXXFLAGS} \
#   -Werror \
#   -DCHIMERA_PATH=:${SOURCE}/stdlib/ \
#   -Wno-c++11-extensions \
#   -Wno-c++98-compat-pedantic \
#   -Wno-date-time \
#   -Wno-padded \
#   -ftemplate-depth=1536"
# export COMPILE="${CXX} ${CPPFLAGS} ${CXXFLAGS}"
# export FORMAT='clang-format -style=file -i'
# export TIDY="clang-tidy -p=${SOURCE} -fix -fix-errors -format-style=file -config='' -header-filter='\.\*'"
# apk add --update jq
#
# {
#   for dir in library source tools unit_tests; do
#     for file in "${SOURCE}/${dir}"/*.cpp "${SOURCE}/${dir}"/**/*.cpp; do
#       jq -n --arg command "${CXX} ${CPPFLAGS} ${CXXFLAGS} -c ${file}" --arg directory "${SOURCE}" --arg file "${file}" '{"command": $command, "directory": $directory, "file": $file}'
#     done
#   done
# } | jq '.'
