#!/usr/bin/env bash

set -ex -o pipefail

root="$(git rev-parse --show-toplevel)"
env="${root}/env"

rm -rf "${env}"
"$1" -m venv "${env}"
export PATH="${env}/bin:${PATH}"
pip3="${env}/bin/pip3"
"${pip3}" install --upgrade pip setuptools wheel
"${pip3}" install -r "${root}/requirements.txt"
"${pip3}" freeze | grep -v -e '^pkg_resources==' >"${root}/requirements.txt"
