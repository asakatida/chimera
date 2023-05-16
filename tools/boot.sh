#!/usr/bin/env bash

set -ex -o pipefail

git submodule update --init

cd "$(git rev-parse --show-toplevel || true)"

case "$(uname)" in
  Darwin )
    brew install python3
    export PATH="${PATH}:${HOME}/.pyenv/shims"
    ;;
  * )
    uname
    echo 'unknown os, failed installation'
    exit
    ;;
esac

python_bin="$(command -v python3)"
"${python_bin}" tools/venv.py
env/bin/ansible-playbook tools/boot.yml
python_bin="$(command -v python3.12)"
"${python_bin}" tools/venv.py
export PATH="${PWD}/env/bin:${PATH}"

cmakelint

chimera_path="${PWD}/stdlib:$(python3 tools/chimera_path.py)"

export CC="${CC:-clang}"
export CXX="${CXX:-clang++}" CXXFLAGS="${CXXFLAGS} -DCHIMERA_PATH=${chimera_path}"

export LLVM_PROFILE_FILE="${PWD}/build/coverage/llvm-profile.%c%p.profraw"

env \
  CMAKE_BUILD_TYPE=Debug \
  tools/cmake.sh "${PWD}/build/debug" -Wdev -Werror=dev

env \
  CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-MinSizeRel}" \
  CXXFLAGS="${CXXFLAGS} -DNDEBUG" \
  tools/cmake.sh "${PWD}/build/release"

tools/lint.sh

ninja -C build/debug -j1 -k0 || true
ninja -C build/release -j1 -k0 || true

python3 tools/supported_distros.py

ansible-playbook tools/docker.yml
