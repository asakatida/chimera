# Copyright (c) 2022 Asa Katida <github@holomaplefeline.net>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""corpus_trim.py"""

from os import chdir
from pathlib import Path
from re import match
from subprocess import DEVNULL, CalledProcessError, run

from tqdm import tqdm  # type: ignore


def cmd(*args: str) -> None:
    run(args, check=True, stdout=DEVNULL)


def main() -> None:
    chdir(Path(__file__).parent.parent)
    cmd("git", "fetch", "--all", "--tags")
    cmd("git", "remote", "prune", "origin")
    try:
        cmd("git", "switch", "corpus")
    except CalledProcessError:
        cmd("git", "switch", "-c", "corpus")
    cmd("git", "rebase", "origin/HEAD")
    cmd("git", "reset", "origin/HEAD")
    cmd("git", "add", ".")
    cmd("git", "commit", "--allow-empty", "-m", "Update fuzzing corpus.")
    cmd("git", "reset", "origin/HEAD")
    for branch in tqdm(
        filter(
            lambda branch: match(r"\w+/regression-.+-refs/\w", branch),
            map(
                str.strip,
                run(
                    ["git", "branch", "-r"], capture_output=True, check=True, text=True
                ).stdout.splitlines(),
            ),
        )
    ):
        cmd("git", "restore", "--source", branch, "--staged", "unit_tests/fuzz")
        cmd("git", "restore", "--worktree", "unit_tests/fuzz")
        cmd("git", "add", "unit_tests/fuzz")
        cmd("git", "commit", "--amend", "--no-edit")
    cmd("env/bin/python3", "tools/corpus_trim.py")
    cmd("git", "add", "unit_tests/fuzz")
    cmd("git", "commit", "--amend", "--no-edit")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print()
