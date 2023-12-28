#!/usr/bin/env sh

set -ex

git clean -dfx
rm -rf "$HOME/.cargo"

git submodule update --init

curl https://sh.rustup.rs -sSf | sh -s -- -y --no-modify-path
set +ex
. "$HOME/.cargo/env"
set -ex
rustup default stable
cargo --version

python3 tools/venv.py venv
set +ex
. venv/bin/activate
set -ex
python3 --version

cmake \
    -DBUILD_DOCS=OFF \
    -DBUILD_DOCUMENTATION=OFF \
    -DCATKIN_ENABLE_TESTING=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_FLAGS= \
    -DCMAKE_CXX_FLAGS= \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCTEST_PARALLEL_LEVEL=3 \
    -B . \
    -S .

make generated/fuzz-1.cpp.o chimera-number-header
