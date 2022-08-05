#!/bin/bash

set -ex -o pipefail
{
  git branch
  git diff
  git stash list
  git log --all --date-order --oneline --graph "$@"
} 2>&1 | less

