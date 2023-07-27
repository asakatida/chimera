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
from asyncio.subprocess import DEVNULL, PIPE
from collections import deque
from sys import argv
from typing import Iterable, TypeVar

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, chunks, cmd, main, splitlines
from corpus_utils import c_tqdm, corpus_creations, corpus_trim

T = TypeVar("T")


async def git_cmd(*args: object) -> None:
    await cmd("git", *args, log=False, out=DEVNULL)


def last(iterable: Iterable[T]) -> T:
    return deque(iterable, maxlen=1)[0]


async def git_restore(sha: str, *paths: object) -> None:
    try:
        for files in chunks(paths, 4096):
            await git_cmd("restore", "--source", sha, *files)
    except ProcessError:
        return
    corpus_trim(disable_bars=True)
    deleted = tuple(
        map(
            lambda line: line.split(maxsplit=1)[-1].strip(),
            filter(
                lambda line: line.startswith(" D "),
                splitlines(
                    await cmd("git", "status", "--porcelain", out=PIPE, log=False)
                ),
            ),
        )
    )
    if deleted:
        await as_completed(
            map(lambda files: git_cmd("restore", *files), chunks(deleted, 4096)),
            cancel=True,
            limit=4,
        )


async def corpus_gather(*paths: str, disable_bars: bool) -> None:
    for sha, files in c_tqdm(await corpus_creations(*paths), "Commits", disable_bars):
        await git_restore(sha.decode(), *files)
        corpus_trim(disable_bars=True)


async def corpus_gather_stable(disable_bars: bool) -> None:
    path = "unit_tests/fuzz/corpus"
    await corpus_gather(path, disable_bars=disable_bars)
    corpus_trim(disable_bars=disable_bars)


def corpus_gather_main(ref: str) -> None:
    if ref == "refs/heads/stable":
        run(corpus_gather_stable(disable_bars=False))


if __name__ == "__main__":
    with main():
        corpus_gather_main(*argv[1:])
