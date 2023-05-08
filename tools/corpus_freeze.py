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
from json import dumps, loads
from pathlib import Path
from sys import argv

from asyncio_cmd import cmd, main
from corpus_ascii import corpus_ascii
from corpus_utils import sha


async def corpus_freeze(output: str) -> None:
    file = Path(output)
    cases = loads(file.read_text())
    cases.update(
        {
            sha(file): file.read_text()
            for file in sorted(corpus_ascii())
            if await cmd(
                "git", "log", "--all", "--oneline", "^HEAD", "--", file, log=False
            )
        }
    )
    file.write_text(dumps(cases, indent=4, sort_keys=True))


if __name__ == "__main__":
    with main():
        run(corpus_freeze(*argv[1:]))
