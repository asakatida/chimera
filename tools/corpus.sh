#!/usr/bin/env bash

set -ex -o pipefail

case $# in
    0 )
      cd "$(git rev-parse --show-toplevel || true)"

      git fetch --all --tags
      git remote prune origin
      git switch corpus
      git rebase origin/HEAD
      git reset origin/HEAD
      git add .
      git commit --allow-empty -m 'Update fuzzing corpus.'

      git branch -r | \
        grep -Fv -e '/HEAD' | \
        grep -F -e '-refs/' | \
        xargs -L1 -- "$0"

      tools/corpus_trim.sh

      git add unit_tests/fuzz
      git commit --amend --no-edit
      ;;
    1 )
      git restore --source="$1" unit_tests/fuzz
      git restore unit_tests/fuzz
      git add unit_tests/fuzz
      git commit --amend --no-edit
      ;;
    * )
      echo "Usage: $0" >&2
      exit 1
      ;;
esac
