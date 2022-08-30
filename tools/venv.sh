#!/usr/bin/env -vS -i bash

set -ex -o pipefail

cd "$(git rev-parse --show-toplevel || true)"

rm -rf env
"$1" -m venv env
export PATH="${PWD}/env/bin:${PATH}"
pip3=env/bin/pip3
"${pip3}" install --upgrade pip setuptools wheel
"${pip3}" install -r requirements.txt
"${pip3}" freeze | grep -v -e '^pkg_resources==' >requirements.txt
