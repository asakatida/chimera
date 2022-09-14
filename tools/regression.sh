#!/usr/bin/env bash

set -ex -o pipefail

find "$2" -type f -print0 | xargs -0 -- "./$1"
