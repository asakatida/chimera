#!/usr/bin/env bash

set -ex -o pipefail

build="$1"
shift
root="$(git rev-parse --show-toplevel)"

env \
  CMAKE_EXPORT_COMPILE_COMMANDS=YES \
  cmake \
  -DBUILD_SHARED_LIBS=OFF \
  -GNinja \
  -S "${root}" \
  -B "${build}" \
  "$@"
