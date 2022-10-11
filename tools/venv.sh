#!/usr/bin/env bash

set -e -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

env -vS -i tools/venv_.sh "$@"
