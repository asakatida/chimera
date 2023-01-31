#!/usr/bin/env bash

set -e -o pipefail

rm -rf env
"$1" -m venv env
env/bin/pip install -r requirements.core.txt
env/bin/pip install -r requirements.txt
