#!/usr/bin/env bash

set -e -o pipefail

rm -rf env
"$1" -m venv env
env/bin/pip3 install --upgrade pip setuptools wheel
env/bin/pip3 install -r requirements.txt
env/bin/pip3 freeze | grep -v -e '^pkg_resources==' >requirements.txt
