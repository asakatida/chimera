# Copyright (c) 2023 Asa Katida <github@holomaplefeline.net>
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

"""corpus_retest.py"""

from asyncio import run
from os import environ
from pathlib import Path
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd_check, cmd_no_timeout
from corpus_trim import conflicts, corpus_trim
from corpus_utils import corpus_merge, gather_paths
from ninja import ninja

IN_CI = environ.get("CI", "") == "true"
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CORPUS_ORIGINAL = FUZZ / "corpus_original"


def rmdir(path: Path) -> None:
    if path.is_dir():
        for child in path.iterdir():
            rmdir(child)
        path.rmdir()
    else:
        path.unlink(missing_ok=True)


async def corpus_retest(build: str) -> None:
    await ninja(build)
    while await cmd_check(
        "ninja", "-C", build, "-j3", "-k0", "regression-log", timeout=1200
    ):
        print(
            "Regression failed, retrying with",
            len(list(filter(Path.is_file, Path("unit_tests/fuzz/corpus").rglob("*")))),
            file=stderr,
        )
    if IN_CI:
        rmdir(CORPUS_ORIGINAL)
    CORPUS.rename(CORPUS_ORIGINAL)
    CORPUS.mkdir(parents=True, exist_ok=True)
    errors = await corpus_merge(CORPUS_ORIGINAL)
    if errors:
        error = errors.pop()
        if errors:
            print("Extra Errors:", *errors, file=stderr, sep="\n")
        raise error


async def regression(build: str) -> None:
    await cmd_no_timeout("ninja", "-C", build, "-j1", "regression")


def trim() -> None:
    """trim corpus"""
    conflicts(gather_paths())
    corpus_trim()


if __name__ == "__main__":
    try:
        trim()
        run(corpus_retest(*argv[1:]))
        trim()
        run(regression(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
