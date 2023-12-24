#!/usr/bin/env sh

set -ex

curl https://sh.rustup.rs -sSf | sh -s -- -y --no-modify-path
export PATH="/home/github/.cargo/bin:${PATH}"
rustup default stable
cargo --version

python3 tools/venv.py venv
source venv/bin/activate
python3 --version

cmake \
    -DBUILD_DOCS=OFF \
    -DBUILD_DOCUMENTATION=OFF \
    -DCATKIN_ENABLE_TESTING=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_FLAGS= \
    -DCMAKE_CXX_FLAGS= \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCTEST_PARALLEL_LEVEL=3
    -B .
