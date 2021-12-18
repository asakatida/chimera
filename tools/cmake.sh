#!/usr/bin/env bash

set -ex -o pipefail

source="$(pwd)"

export CXXFLAGS
export LDFLAGS

mkdir -p "$1"
cd "$1"
shift

cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_SHARED_LIBS=OFF \
  "$@" \
  "${source}"
