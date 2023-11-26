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
from hashlib import sha256
from pathlib import Path
from subprocess import PIPE
from sys import argv

from asyncio_as_completed import as_completed
from asyncio_cmd import cmd, main
from corpus_utils import c_tqdm, corpus_objects, corpus_trim, gather_paths


async def corpus_gather(*paths: str, disable_bars: bool | None) -> None:
    for path in paths:
        for case in await corpus_objects(
            path,
            disable_bars=disable_bars,
            exclude=set(
                map(
                    str.strip,
                    map(
                        bytes.decode,
                        await as_completed(
                            c_tqdm(
                                map(
                                    lambda case: cmd(
                                        "git", "hash-object", case, out=PIPE, log=False
                                    ),
                                    gather_paths(),
                                ),
                                "Gather existing corpus objects",
                                disable_bars,
                            )
                        ),
                    ),
                )
            ),
        ):
            new_file = Path(path) / sha256(case).hexdigest()
            new_file.parent.mkdir(parents=True, exist_ok=True)
            new_file.write_bytes(case)
    corpus_trim(disable_bars=disable_bars)


def corpus_gather_main(ref: str) -> None:
    if ref == "refs/heads/stable":
        run(
            corpus_gather(
                "unit_tests/fuzz/crashes", "unit_tests/fuzz/corpus", disable_bars=False
            )
        )


if __name__ == "__main__":
    with main():
        corpus_gather_main(*argv[1:])
