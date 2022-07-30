#!/usr/bin/env bash

set -ex -o pipefail

env

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"
build_root="${root}/build"
debug_root="${build_root}/debug"
release_root="${build_root}/release"

"${scripts}/shellcheck.sh"

export PATH="${root}/env/bin:${PATH}"

"${scripts}/lint.sh"

ninja -C "${debug_root}"
ninja -C "${debug_root}"
ninja -C "${release_root}"
ninja -C "${debug_root}" check
ninja -C "${debug_root}" check_rand
ninja -C "${debug_root}" corpus-num
ninja -C "${debug_root}" corpus
ninja -C "${debug_root}" spec
ninja -C "${release_root}" spec

docker build -t chimera/base "tools/docker/base/"
docker build -t chimera/clang "tools/docker/clang/"
docker build -t chimera/gcc "tools/docker/gcc/"

docker-exec chimera/clang "tools/clang-fix.sh"
docker-exec chimera/gcc "tools/gcc-sanity.sh"

docker-exec chimera/clang "tools/ninja-intense.sh build/clang"
docker-exec chimera/gcc "tools/ninja-intense.sh build/gcc"
