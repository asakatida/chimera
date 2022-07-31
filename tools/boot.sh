#!/usr/bin/env bash

set -ex -o pipefail

git submodule update --init

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"
build_root="${root}/build"

sudo apt-get install --yes ccache python3.9-venv shellcheck

"${scripts}/shellcheck.sh"

"${scripts}/venv.sh" "$(command -v python3.9)"

"${root}/env/bin/ansible-playbook" "${scripts}/boot.yml"

"${scripts}/venv.sh" "$(command -v python3.12)"

export PATH="${root}/env/bin:${PATH}"

cmakelint

chimera_path="${root}/stdlib:$(python3 "${scripts}/chimera_path.py")"

export CC='ccache clang' CFLAGS="${CFLAGS} -DCHIMERA_PATH=${chimera_path}"
export CXX='ccache clang++' CXXFLAGS="${CFLAGS} ${CXXFLAGS} -std=c++2b"

env \
  CFLAGS="${CFLAGS} -Wall -Wpedantic -Werror" \
  CXXFLAGS="${CXXFLAGS} -Wall -Wpedantic -Werror" \
  CMAKE_EXPORT_COMPILE_COMMANDS=YES \
  cmake -DCMAKE_BUILD_TYPE=Debug -Wdev -Werror=dev -GNinja -S "${root}" -B "${build_root}/debug"

env \
  CFLAGS="${CFLAGS} -DNDEBUG" \
  CXXFLAGS="${CXXFLAGS} -DNDEBUG" \
  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_BUILD_TYPE=MinSizeRel -GNinja -S "${root}" -B "${build_root}/release"

"${scripts}/lint.sh"

python3 "${scripts}/supported_distros.py"

ansible-playbook "${scripts}/docker.yml"
