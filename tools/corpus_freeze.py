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
from corpus_utils import c_tqdm, corpus_objects

BAD_KEYS = {
    "1e445d9da094d2116c048d181b9d6247f497da6e",
    "24c6055128d9a1ed7ce5b56bcd2ff9d94c2e181d",
    "457e017b76fc853792ef9f304b5b09d7b5c1e945",
    "4a2e15dba7894cda9fdf6da2b204b0806fc45d95",
    "4c8b5364cfa29a8a932c60061863c327aea88cce",
    "61f28df9fed06b02a333a1decc867334c87fe57c",
    "622f0e464d3228e2b9358fca0353f5683bb2b0e6",
    "86385647fee3b7563c07304e27fb7d02f5a2f505",
    "932259c1ee3ec10a70d2a4fa0398a4efe951feb7",
    "b81b8a2844fa7d4c9709df53a215140cd6dcdab3",
    "b91abaaa6ed804eb88871fcabf03bb8ec6b1c480",
    "da3c0f8dd4447bc17fb4fb7b6b11569091abdc11",
    "dd27e49664cf1b1c6a35ae5c1e195c1d86be2a33",
    "dfb1f01c256513a162d01ce5693b3a428b4e3a03",
    "eea02be05b33e625f43ac08dcb76ed3e9ab3fb1e",
    "5875a1f00d22807731af954945ff86bd828b8c64",
    "3a814cc440c7f2db306065b94a6213620e3ebe6b",
    "a23ea7021107ed9e49d90c6fcb136291d8c73914",
    "514f4c42b62188812a76275ac02b91d736c535ed",
}


async def corpus_freeze(output: str, disable_bars: bool | None) -> None:
    file = Path(output)
    with file.open() as istream:
        cases = {key: value for key, value in load(istream).items()}
    crashes = [
        path for path in Path("unit_tests/fuzz/crashes").rglob("*") if path.is_file()
    ]
    existing = {
        key.strip().decode()
        for key in await as_completed(
            c_tqdm(
                (
                    cmd("git", "hash-object", path, log=False, out=PIPE)
                    for path in crashes
                ),
                "Hash corpus",
                disable_bars,
                total=len(crashes),
            )
        )
    }
    for key in BAD_KEYS.union(existing).intersection(cases.keys()):
        del cases[key]
    cases.update(
        (sha, b64encode(compress(obj)).decode())
        for sha, obj in await corpus_objects(
            "unit_tests/fuzz/corpus",
            "unit_tests/fuzz/crashes",
            disable_bars=disable_bars,
            exclude=BAD_KEYS.union(existing).union(cases.keys()),
        )
        if is_ascii(obj)
    )
    with file.open("w") as ostream:
        dump(cases, ostream, indent=4, sort_keys=True)


async def corpus_freeze_main(output: str) -> None:
    await corpus_freeze(output, disable_bars=None)


if __name__ == "__main__":
    with main():
        run(corpus_freeze_main(*argv[1:]))
