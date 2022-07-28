#!/usr/bin/env bash

set -ex -o pipefail

sudo apt-get install --yes python3.9-venv shellcheck

root="$(git rev-parse --show-toplevel)"
scripts="${root}/tools"
build="${root}/build"
base_python="$(command -v python3.9)"

"${scripts}/shellcheck.sh"

clang_version="${1:-15}"

"${base_python}" -m venv "${root}/env"
export PATH="${root}/env/bin:${PATH}"
pip3 install -r "${root}/requirements.txt"
pip3 freeze > "${root}/requirements.txt"

cmakelint

git submodule update --init

find "${root}" -name '*.py' -print0 | xargs -0 -- git ls-tree --full-tree --name-only -z HEAD -- | xargs -0 -- isort --python-version auto
find "${root}" -name '*.py' -print0 | xargs -0 -- git ls-tree --full-tree --name-only -z HEAD -- | xargs -0 -- black --preview --target-version py310
find "${root}" -name '*.py' -print0 | xargs -0 -- git ls-tree --full-tree --name-only -z HEAD -- | xargs -0 -- pylama
find "${root}" -not -path "${root}/stdlib/*" -name '*.py' -print0 | xargs -0 -- git ls-tree --full-tree --name-only -z HEAD -- | xargs -0 -- mypy

chimera_path="${root}/stdlib:$("${base_python}" "${scripts}/chimera_path.py")"

export CMAKE_EXPORT_COMPILE_COMMANDS=YES
export CC="clang-${clang_version}" CFLAGS="-Wall -Wpedantic -Werror -DCHIMERA_PATH=${chimera_path}"
export CXX="clang++-${clang_version}" CXXFLAGS="${CFLAGS}"

cmake -GNinja -B "${build}"
ninja -C "${build}"
python3 "${scripts}/generate_utf8_id_continue.py"
python3 "${scripts}/generate_utf8_id_start.py"
python3 "${scripts}/generate_utf8_space.py"
"${scripts}/clang-format.sh"
"${scripts}/clang-tidy.sh" "${build}"
ninja -C "${build}" check
ninja -C "${build}" check_rand
ninja -C "${build}" corpus-num
ninja -C "${build}" corpus
ninja -C "${build}" spec

docker build -t chimera/base "${scripts}/docker/base/"
docker build -t chimera/clang "${scripts}/docker/clang/"
docker build -t chimera/gcc "${scripts}/docker/gcc/"

docker-exec chimera/clang "${scripts}/clang-fix.sh"
docker-exec chimera/gcc "${scripts}/gcc-sanity.sh"

docker-exec chimera/clang "${scripts}/ninja-intense.sh build/clang"
docker-exec chimera/gcc "${scripts}/ninja-intense.sh build/gcc"
