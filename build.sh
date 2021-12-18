#!/usr/bin/env bash

set -ex -o pipefail

find . '(' -not -path './external/*' ')' -and -name '*.sh' -print0 | xargs -0 -- shellcheck

docker build -t chimera/base tools/docker/base/
docker build -t chimera/clang tools/docker/clang/
docker build -t chimera/gcc tools/docker/gcc/

docker-exec chimera/clang tools/clang-fix.sh
docker-exec chimera/gcc tools/gcc-sanity.sh

docker-exec chimera/clang tools/ninja-intense.sh build/clang
docker-exec chimera/gcc tools/ninja-intense.sh build/gcc
