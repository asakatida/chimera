#!/usr/bin/env sh

set -ex

env | cut -f1 -d= | sort -u

cd "$(git rev-parse --show-toplevel || true)"
debug_root=build/debug
release_root=build/release

export PATH="${PWD}/env/bin:${PATH}"

tools/lint.sh

export LLVM_PROFILE_FILE="${PWD}/build/coverage/llvm-profile.%c%p.profraw"

python tools/ninja.py "${debug_root}"
python tools/ninja.py "${debug_root}"
python tools/ninja.py "${release_root}"
ctest --build-and-test . "${debug_root}" --build-generator Ninja --build-noclean
python tools/ninja.py "${debug_root}" regression
python tools/ninja.py "${debug_root}" corpus
python tools/ninja.py "${debug_root}" spec
python tools/ninja.py "${release_root}" spec

docker build -t chimera/base "tools/docker/base/"
docker build -t chimera/clang "tools/docker/clang/"
docker build -t chimera/gcc "tools/docker/gcc/"

docker-exec chimera/clang "tools/clang-fix.sh"
docker-exec chimera/gcc "tools/gcc-sanity.sh"

docker-exec chimera/clang "tools/ninja-intense.sh build/clang"
docker-exec chimera/gcc "tools/ninja-intense.sh build/gcc"
