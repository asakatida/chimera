#!/usr/bin/env sh

set -ex

cd "$(git rev-parse --show-toplevel || true)"

scan-build tools/debug-onshot.sh
scan-build --use-cc clang --use-c++ clang++ tools/debug-onshot.sh
