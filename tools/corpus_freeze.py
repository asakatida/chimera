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
from hashlib import sha256
from itertools import chain
from json import dumps, loads
from pathlib import Path
from sys import argv

from asyncio_as_completed import as_completed
from asyncio_cmd import chunks, cmd, main, splitlines
from corpus_ascii import corpus_ascii, is_ascii
from corpus_utils import c_tqdm, corpus_creations


async def corpus_changes(cases: dict[str, str], disable_bars: bool) -> list[str]:
    return [
        file.read_text()
        for file in filter(
            lambda file: sha256(file.read_bytes()).hexdigest() not in cases,
            c_tqdm(corpus_ascii(), "ascii", disable_bars),
        )
        if await cmd("git", "log", "--all", "--oneline", "^HEAD", "--", file, log=False)
    ]


async def crash_contents(disable_bars: bool) -> list[bytes]:
    return await as_completed(
        map(
            lambda sha: cmd("git", "cat-file", "-p", sha, log=False),
            c_tqdm(
                chain.from_iterable(
                    map(
                        splitlines,
                        chain.from_iterable(await crash_objects(disable_bars)),
                    )
                ),
                "Files",
                disable_bars,
            ),
        )
    )


async def crash_objects(disable_bars: bool) -> list[list[bytes]]:
    return await as_completed(
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
                    ),
                    chunks(item[1], 4096),
                )
            ),
            c_tqdm(
                (await corpus_creations("unit_tests/fuzz/crashes")).items(),
                "Commits",
                disable_bars,
            ),
        )
    )


async def corpus_freeze(output: str, disable_bars: bool) -> None:
    file = Path(output)
    cases = loads(file.read_text())
    cases.update(
        map(
            lambda case: (sha256(case.encode()).hexdigest(), case),
            await corpus_changes(cases, disable_bars),
        )
    )
    cases.update(
        map(
            lambda case: (sha256(case).hexdigest(), case.decode()),
            filter(is_ascii, await crash_contents(disable_bars)),
        )
    )
    file.write_text(dumps(cases, indent=4, sort_keys=True))


def corpus_freeze_main(output: str) -> None:
    run(corpus_freeze(output, disable_bars=False))


if __name__ == "__main__":
    with main():
        corpus_freeze_main(*argv[1:])
