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

"""corpus_gather.py"""

from asyncio import run
from asyncio.subprocess import PIPE
from pathlib import Path
from sys import stderr

from asyncio_cmd import ProcessError, cmd
from tqdm import tqdm  # type: ignore

FUZZ = Path(__file__).resolve().parent.parent / "unit_tests" / "fuzz"
FUZZ_DIRS = tuple(map(str, (FUZZ / "crashes", FUZZ / "corpus", FUZZ / "dictionaries")))


async def main() -> None:
    await cmd("git", "fetch", "--all", "--tags", timeout=600)
    await cmd("git", "remote", "prune", "origin", timeout=600)
    await cmd("git", "add", *FUZZ_DIRS)
    await cmd("git", "commit", "--allow-empty", "-m", "WIP")
    stdout = await cmd(
        "git",
        "log",
        "--all",
        "--format=%h",
        "^HEAD",
        "--",
        *FUZZ_DIRS,
        stdout=PIPE,
    )
    for sha in tqdm(
        map(
            str.strip,
            stdout.decode().splitlines(),
        ),
        desc="Branches",
        unit_scale=True,
    ):
        await cmd("git", "restore", "--source", sha, "--staged", *FUZZ_DIRS, log=False)
        await cmd("git", "restore", "--worktree", str(FUZZ), log=False)
        await cmd("git", "add", str(FUZZ), log=False)
        await cmd("git", "commit", "--amend", "--no-edit", log=False)
    await cmd("git", "reset", "HEAD^", timeout=600)


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
