#!/usr/bin/env bash

set -ex -o pipefail

git submodule update --init

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"
build_root="${root}/build"

sudo apt-get install --yes ccache python3.9-venv shellcheck

"${scripts}/shellcheck.sh"

python3.9 -m venv "${root}/env"
export PATH="${root}/env/bin:${PATH}"
pip3 install --upgrade pip setuptools wheel
pip3 install -r "${root}/requirements.txt"
pip3 freeze | grep -v -e '^pkg_resources==' >"${root}/requirements.txt"

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

# ansible-playbook "${scripts}/docker.yml"
