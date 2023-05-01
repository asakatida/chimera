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
from pathlib import Path
from string import printable
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd
from corpus_ascii import corpus_ascii
from corpus_utils import sha

TEST_CASE = """

TEST_CASE(R"(fuzz `{data}`)", "[{tag}]") {{
  auto test_case = R"({data})"sv;
  TestOne(test_case.data(), test_case.size());
}}

""".format


async def corpus_freeze(output: str) -> None:
    file = Path(output)
    current_state = file.read_text()
    with file.open("a") as ostream:
        for file in sorted(corpus_ascii()):
            test_data = "".join(
                map(
                    lambda c: (
                        c
                        if c in printable
                        else "\\x"
                        + "\\x".join(
                            map(lambda h: h[2:].rjust(2, "0"), map(hex, c.encode()))
                        )
                    ),
                    file.read_text(),
                )
            )
            if len(test_data) > 120 or test_data in current_state:
                continue
            if await cmd(
                "git",
                "log",
                "--all",
                "--format=%H",
                "^origin/HEAD",
                "--",
                file,
                err=PIPE,
                log=False,
                out=PIPE,
            ):
                ostream.write(TEST_CASE(data=test_data, tag=sha(file)))


if __name__ == "__main__":
    try:
        run(corpus_freeze(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
