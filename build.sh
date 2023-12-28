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

cmake -DCMAKE_BUILD_TYPE=Release -B . -S .
make -j8
