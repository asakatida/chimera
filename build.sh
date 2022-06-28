#!/usr/bin/env bash

set -ex -o pipefail

tools/shellcheck.sh

docker build -t chimera/base tools/docker/base/
docker build -t chimera/clang tools/docker/clang/
docker build -t chimera/gcc tools/docker/gcc/

docker-exec chimera/clang tools/clang-fix.sh
docker-exec chimera/gcc tools/gcc-sanity.sh

docker-exec chimera/clang tools/ninja-intense.sh build/clang
docker-exec chimera/gcc tools/ninja-intense.sh build/gcc
