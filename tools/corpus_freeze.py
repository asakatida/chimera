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
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd
from corpus_ascii import corpus_ascii

TEST_CASE = """

TEST_CASE(R"(fuzz `{data}`)") {{
  auto test_case = R"({data})"sv;
  TestOne(test_case.data(), test_case.size());
}}

""".format


async def corpus_freeze(output: str) -> None:
    file = Path(output)
    paths = sorted(corpus_ascii())
    current_state = file.read_text()
    with file.open("a") as ostream:
        for file in paths:
            test_data = repr(file.read_text())[1:-1]
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
                ostream.write(TEST_CASE(data=test_data))


if __name__ == "__main__":
    try:
        run(corpus_freeze(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
