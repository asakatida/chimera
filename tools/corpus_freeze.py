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
from base64 import b64encode
from json import dump, load
from lzma import compress
from pathlib import Path
from subprocess import PIPE
from sys import argv

from asyncio_as_completed import as_completed
from asyncio_cmd import cmd, main
from corpus_ascii import is_ascii
from corpus_utils import c_tqdm, corpus_objects, gather_paths


async def corpus_hash(case: bytes) -> tuple[str, str]:
    return (
        (
            await cmd(
                "git",
                "hash-object",
                "--stdin",
                input=case,
                log=False,
                out=PIPE,
            )
        )
        .strip()
        .decode(),
        b64encode(compress(case)).decode(),
    )


async def corpus_freeze(output: str, disable_bars: bool | None) -> None:
    file = Path(output)
    with file.open() as istream:
        cases_orig = dict(load(istream))
    cases = dict(
        await as_completed(
            c_tqdm(
                map(
                    corpus_hash,
                    filter(
                        is_ascii,
                        await corpus_objects(
                            "unit_tests/fuzz/corpus",
                            "unit_tests/fuzz/crashes",
                            disable_bars=disable_bars,
                            exclude=set(cases_orig.keys()),
                        ),
                    ),
                ),
                "Hash corpus",
                disable_bars,
            )
        )
    )
    existing = dict(
        await as_completed(
            c_tqdm(
                map(corpus_hash, map(Path.read_bytes, gather_paths())),
                "Hash corpus",
                disable_bars,
            )
        )
    )
    cases = dict(filter(lambda case: case[0] not in existing, cases.items()))
    cases_orig.update(cases)
    for key in set(existing).intersection(cases_orig.keys()):
        del cases_orig[key]
    with file.open("w") as ostream:
        dump(cases_orig, ostream, indent=4, sort_keys=True)


def corpus_freeze_main(output: str) -> None:
    run(corpus_freeze(output, disable_bars=None))


if __name__ == "__main__":
    with main():
        corpus_freeze_main(*argv[1:])
