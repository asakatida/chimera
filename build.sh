#!/usr/bin/env bash

set -ex -o pipefail

env | cut -f1 -d= | sort -u

cd "$(git rev-parse --show-toplevel || true)"
debug_root=build/debug
release_root=build/release

tools/shellcheck.sh

export PATH="${PWD}/env/bin:${PATH}"

tools/lint.sh

export LLVM_PROFILE_FILE="${PWD}/build/coverage/llvm-profile.%c%p.profraw"

tools/ninja.sh "${debug_root}"
tools/ninja.sh "${debug_root}"
tools/ninja.sh "${release_root}"
tools/ninja.sh "${debug_root}" check-rand
tools/ninja.sh "${debug_root}" regression
tools/ninja.sh "${debug_root}" corpus
tools/ninja.sh "${debug_root}" spec
tools/ninja.sh "${release_root}" spec

docker build -t chimera/base "tools/docker/base/"
docker build -t chimera/clang "tools/docker/clang/"
docker build -t chimera/gcc "tools/docker/gcc/"

docker-exec chimera/clang "tools/clang-fix.sh"
docker-exec chimera/gcc "tools/gcc-sanity.sh"

docker-exec chimera/clang "tools/ninja-intense.sh build/clang"
docker-exec chimera/gcc "tools/ninja-intense.sh build/gcc"
