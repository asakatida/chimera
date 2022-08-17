#!/usr/bin/env bash

set -ex -o pipefail

apt-get update
apt-get install --yes \
  apt-transport-https \
  apt-utils \
  build-essential \
  ca-certificates \
  ccache \
  cmake \
  curl \
  docker \
  fish \
  git \
  gnupg \
  gnupg-agent \
  lsb-release \
  ninja-build \
  python3.10-venv \
  shellcheck \
  software-properties-common \
  sudo \
  wget
apt-get clean
rm -rf /var/cache/apt/* /var/lib/apt/lists/*

fish_cmd="$(command -v fish)"
chsh -s "${fish_cmd}"
fish -c 'exit'

curl https://apt.llvm.org/llvm.sh -o llvm.sh
chmod +x llvm.sh
./llvm.sh 15 all
rm llvm.sh
clang___cmd="$(command -v clang++-15)"
clang_cmd="$(command -v clang-15)"
clang_format_cmd="$(command -v clang-format-15)"
clang_tidy_cmd="$(command -v clang-tidy-15)"
scan_build_cmd="$(command -v scan-build-15)"
ln -s "${clang___cmd}" /usr/local/bin/clang++
ln -s "${clang_cmd}" /usr/local/bin/clang
ln -s "${clang_format_cmd}" /usr/local/bin/clang-format
ln -s "${clang_tidy_cmd}" /usr/local/bin/clang-tidy
ln -s "${scan_build_cmd}" /usr/local/bin/scan-build
