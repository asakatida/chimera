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

"""corpus_freeze.py"""

from asyncio import run
from asyncio.subprocess import PIPE
from base64 import b64decode, b64encode
from hashlib import sha256
from itertools import chain
from json import dump, load
from pathlib import Path
from sys import argv
from typing import Iterable

from asyncio_as_completed import as_completed
from asyncio_cmd import chunks, cmd, main, splitlines
from chimera_utils import IN_CI
from corpus_ascii import is_ascii
from corpus_utils import c_tqdm, corpus_creations, gather_paths, sha


async def corpus_changes(disable_bars: bool | None) -> Iterable[bytes]:
    return filter(
        is_ascii, await corpus_objects("unit_tests/fuzz/corpus", disable_bars)
    )


async def crash_contents(disable_bars: bool | None) -> list[bytes]:
    return await corpus_objects("unit_tests/fuzz/crashes", disable_bars)


async def corpus_objects(path: str, disable_bars: bool | None) -> list[bytes]:
    return await as_completed(
        map(
            lambda sha: cmd("git", "cat-file", "-p", sha, log=False, out=PIPE),
            c_tqdm(
                set(
                    chain.from_iterable(
                        map(
                            splitlines,
                            chain.from_iterable(
                                await as_completed(
                                    map(
                                        lambda item: as_completed(
                                            map(
                                                lambda chunk: cmd(
                                                    "git",
                                                    "ls-tree",
                                                    "--full-tree",
                                                    "--object-only",
                                                    "-r",
                                                    item[0].decode(),
                                                    *chunk,
                                                    log=False,
                                                    out=PIPE,
                                                ),
                                                chunks(item[1], 4096),
                                            )
                                        ),
                                        await corpus_creations(
                                            path, disable_bars=disable_bars
                                        ),
                                    )
                                )
                            ),
                        )
                    )
                ),
                "Files",
                disable_bars,
            ),
        )
    )


async def corpus_freeze(output: str, disable_bars: bool | None) -> None:
    file = Path(output)
    with file.open() as istream:
        cases_orig = dict(load(istream))
    cases = dict(zip(cases_orig.keys(), map(b64decode, cases_orig.values())))
    cases.update(
        map(
            lambda case: (sha256(case).hexdigest(), case),
            cases.values(),
        )
    )
    if not IN_CI:
        cases.update(
            map(
                lambda case: (sha256(case).hexdigest(), case),
                await corpus_changes(disable_bars),
            )
        )
        cases.update(
            map(
                lambda case: (sha256(case).hexdigest(), case),
                await crash_contents(disable_bars),
            )
        )
    for key in set(map(sha, gather_paths())).intersection(cases.keys()):
        del cases[key]
    cases_orig = dict(
        zip(cases.keys(), map(bytes.decode, map(b64encode, cases.values())))
    )
    with file.open("w") as ostream:
        dump(cases_orig, ostream, indent=4, sort_keys=True)


def corpus_freeze_main(output: str) -> None:
    run(corpus_freeze(output, disable_bars=None))


if __name__ == "__main__":
    with main():
        corpus_freeze_main(*argv[1:])
