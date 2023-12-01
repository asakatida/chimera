#!/usr/bin/env sh

set -ex

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
python_bin="$(command -v python3.14)"
"${python_bin}" tools/venv.py
export PATH="${PWD}/env/bin:${PATH}"

cmakelint

chimera_path="${PWD}/stdlib:$(python3 tools/chimera_path.py)"

export CXXFLAGS="${CXXFLAGS} -DCHIMERA_PATH=${chimera_path}"

export LLVM_PROFILE_FILE="${PWD}/build/coverage/llvm-profile.%c%p.profraw"

tools/cmake.sh "${PWD}/build/debug" -Wdev -Werror=dev

env \
  CMAKE_BUILD_TYPE=MinSizeRel \
  CXXFLAGS="${CXXFLAGS} -DNDEBUG" \
  tools/cmake.sh "${PWD}/build/release"

tools/lint.sh

python3 tools/ninja.py build/debug || true
python3 tools/ninja.py build/release || true

python3 tools/supported_distros.py

ansible-playbook tools/docker.yml
