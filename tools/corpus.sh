#!/usr/bin/env bash

set -e -o pipefail

case $# in
    0 )
      set -x
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
        xargs --no-run-if-empty -L1 -- "$0"
      tools/corpus_trim.sh
      git add unit_tests/fuzz
      git commit --amend --no-edit
      ;;
    1 )
      git restore --source="$1" --staged unit_tests/fuzz
      git restore --worktree unit_tests/fuzz
      git add unit_tests/fuzz
      git commit --amend --no-edit
      ;;
    * )
      echo "Usage: $0" >&2
      exit 1
      ;;
esac
