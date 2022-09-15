#!/usr/bin/env bash

xargs --no-run-if-empty --null -- \
  git ls-tree --full-tree --name-only -z "$@" HEAD --
