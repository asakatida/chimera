#!/usr/bin/env bash

set -e -o pipefail

case "$(uname)" in
  Darwin )
    xargs -0 "$@"
    ;;
  * )
    xargs --no-run-if-empty --null "$@"
    ;;
esac
