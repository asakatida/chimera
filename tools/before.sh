#!/usr/bin/env bash

set -e -o pipefail

CC='' CXX='' CXXFLAGS='' /opt/virtualenv/bin/pip install -r requirements.txt | {
  grep --invert-match -e '^Requirement already satisfied:' || true
}
