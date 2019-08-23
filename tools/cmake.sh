#!/usr/bin/env bash

set -lx CXXFLAGS "$CXXFLAGS"
set -lx LDFLAGS "$LDFLAGS"

rm CMakeCache.txt

cmake -GNinja \
-DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DBUILD_SHARED_LIBS=OFF \
.
