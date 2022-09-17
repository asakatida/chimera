#!/usr/bin/env bash

xargs --null -- \
  git ls-tree --full-tree --name-only -z "$@" HEAD --
