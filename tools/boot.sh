#!/usr/bin/env bash

set -ex -o pipefail

env | cut -f1 -d= | sort -u
export PREBUILD_CHECK="${GITPOD_WORKSPACE_URL:-}"

git submodule update --init

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"
build_root="${root}/build"

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

"${scripts}/shellcheck.sh"

python_bin="$(command -v python3)"
"${scripts}/venv.sh" "${python_bin}"

"${root}/env/bin/ansible-playbook" "${scripts}/boot.yml"

python_bin="$(command -v python3.12)"
"${scripts}/venv.sh" "${python_bin}"

export PATH="${root}/env/bin:${PATH}"

cmakelint

chimera_path="${root}/stdlib:$(python3 "${scripts}/chimera_path.py")"

export CC="${CC:-clang}" CFLAGS="${CFLAGS} -DCHIMERA_PATH=${chimera_path}"
export CXX="${CXX:-clang++}" CXXFLAGS="${CXXFLAGS} -DCHIMERA_PATH=${chimera_path}"

env \
  CFLAGS="${CFLAGS} -Wall -Wextra -Wpedantic -Werror" \
  CMAKE_BUILD_TYPE=Debug \
  CXXFLAGS="${CXXFLAGS} -Wall -Wextra -Wpedantic -Werror" \
  "${scripts}/cmake.sh" "${build_root}/debug" -Wdev -Werror=dev

env \
  CFLAGS="${CFLAGS} -DNDEBUG" \
  CMAKE_BUILD_TYPE="{CMAKE_BUILD_TYPE:-MinSizeRel}" \
  CXXFLAGS="${CXXFLAGS} -DNDEBUG" \
  "${scripts}/cmake.sh" "${build_root}/release"

"${scripts}/lint.sh"

python3 "${scripts}/supported_distros.py"

ansible-playbook "${scripts}/docker.yml"
