#!/usr/bin/env bash

set -ex -o pipefail

env | cut -f1 -d= | sort -u
export PREBUILD_CHECK="${GITPOD_WORKSPACE_URL:-}"

git submodule update --init

cd "$(git rev-parse --show-toplevel || true)"

case "$(uname)" in
  Darwin )
    brew install python3 shellcheck
    sudo true
    export PATH="${PATH}:${HOME}/.pyenv/shims"
    ;;
  Linux )
    if command -v apt; then
      echo 'Apt found, expecting Dockerfile is used'
    else
      echo 'No apt found, failed installation'
      exit
    fi
    ;;
  * )
    uname
    echo 'unknown os, failed installation'
    exit
    ;;
esac

tools/shellcheck.sh

python_bin="$(command -v python3)"
tools/venv.sh "${python_bin}"

env/bin/ansible-playbook tools/boot.yml

python_bin="$(command -v python3.12)"
tools/venv.sh "${python_bin}"

export PATH="${PWD}/env/bin:${PATH}"

cmakelint

chimera_path="${PWD}/stdlib:$(python3 tools/chimera_path.py)"

CXXFLAGS="$(echo "${CXXFLAGS}" | xargs)"
export CXX="${CXX:-clang++}" CXXFLAGS="${CXXFLAGS} -DCHIMERA_PATH=${chimera_path}"

export LLVM_PROFILE_FILE="${PWD}/build/coverage/llvm-profile.%c%p.profraw"

env \
  CMAKE_BUILD_TYPE=Debug \
  tools/cmake.sh "${PWD}/build/debug" -Wdev -Werror=dev

env \
  CMAKE_BUILD_TYPE="{CMAKE_BUILD_TYPE:-MinSizeRel}" \
  CXXFLAGS="${CXXFLAGS} -DNDEBUG" \
  tools/cmake.sh "${PWD}/build/release"

tools/lint.sh

python3 tools/supported_distros.py

ansible-playbook tools/docker.yml
