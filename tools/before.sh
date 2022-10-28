#!/usr/bin/env bash

set -e -o pipefail

virtualenv=/opt/virtualenv

user="$(id -u)"
group="$(id -g)"

sudo chown -R "${user}:${group}" "${virtualenv}"
chmod -R go=u-w "${virtualenv}"
"${virtualenv}/bin/pip" install -r requirements.txt | {
  grep --invert-match -e '^Requirement already satisfied:' || true
}
