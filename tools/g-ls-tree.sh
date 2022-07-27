#!/usr/bin/env bash

xargs -0 -- git ls-tree --full-tree --name-only -z "$@" HEAD --
