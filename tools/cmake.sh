#!/usr/bin/env bash

set -ex -o pipefail

build="$1"
shift
root="$(git rev-parse --show-toplevel)"

export CXXFLAGS
export LDFLAGS

cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_SHARED_LIBS=OFF \
  -S "${root}" \
  -B "${build}" \
  "$@"
