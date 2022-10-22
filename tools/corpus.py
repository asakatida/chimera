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
from subprocess import DEVNULL, PIPE, CalledProcessError, TimeoutExpired, run
from sys import stderr

from tqdm import tqdm  # type: ignore

FUZZ_DIRS = (
    "unit_tests/fuzz/crashes",
    "unit_tests/fuzz/corpus",
    "unit_tests/fuzz/dictionaries",
)


def cmd(*args: str) -> None:
    run(args, check=True, stderr=PIPE, stdout=DEVNULL)


def main() -> None:
    chdir(Path(__file__).parent.parent)
    cmd("git", "fetch", "--all", "--tags")
    cmd("git", "remote", "prune", "origin")
    cmd("git", "commit", "--allow-empty", "-m", "WIP")
    for sha in tqdm(
        map(
            str.strip,
            run(
                ["git", "log", "--all", "--format=%h", "^HEAD", "--", *FUZZ_DIRS],
                capture_output=True,
                check=True,
            )
            .stdout.decode()
            .splitlines(),
        ),
        desc="Branches",
        unit_scale=True,
    ):
        cmd("git", "restore", "--source", sha, "--staged", *FUZZ_DIRS)
        cmd("git", "restore", "--worktree", "unit_tests/fuzz")
        cmd("git", "add", "unit_tests/fuzz")
        cmd("git", "commit", "--amend", "--no-edit")
    run(["env/bin/python3", "tools/corpus_trim.py"], check=True)
    cmd("git", "reset", "HEAD^")


if __name__ == "__main__":
    try:
        main()
    except CalledProcessError as error:
        print(*error.cmd, file=stderr)
        print((error.stderr or b"").decode(), file=stderr)
        exit(error.returncode)
    except TimeoutExpired as error:
        print(*error.cmd, file=stderr)
        print((error.stderr or b"").decode(), file=stderr)
        exit(1)
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
