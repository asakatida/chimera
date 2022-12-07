#!/usr/bin/env bash

set -e -o pipefail

rm -rf env
"$1" -m venv env
env/bin/pip install --upgrade pip setuptools wheel
env/bin/pip install -r requirements.txt
env/bin/pip freeze | grep -v -e '^pkg_resources==' >requirements.txt
