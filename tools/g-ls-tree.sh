#!/usr/bin/env bash
# This script is a wrapper around git ls-tree that adds the ability to
# filter by file extension. It is used by the build system to generate
# lists of files to be processed by other tools.

set -e -o pipefail

if [[ "${CI:-}" == "true" ]] || [[ -n "${PREBUILD_CHECK}" ]]; then
  find . "$@" -print0 | \
    tools/xargs.sh -- \
    git ls-tree --full-tree --name-only -z HEAD --
else
  find . "$@" -print0 | \
    tools/xargs.sh -- \
    git ls-tree --full-tree --name-only -z HEAD -- | \
    tools/xargs.sh -- \
    git diff -z --name-only HEAD^ --
fi
