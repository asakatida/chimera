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

"""ninja.py"""

from asyncio import run
from sys import argv

from asyncio_cmd import cmd, main


async def ninja_cmd(
    build: object, *args: object, timeout: int | None = 20 * 60
) -> None:
    await cmd("ninja", "-C", build, *args, timeout=timeout)


async def ninja(build: object, *args: object) -> None:
    await ninja_cmd(build, "-j1", "chimera-grammar")
    await ninja_cmd(build, "-j3", "chimera", "chimera-core", "libchimera")
    await ninja_cmd(build, "Catch2WithMain")
    await ninja_cmd(build, "-j2", "unit-test", timeout=40 * 60)
    await ninja_cmd(build, "-j1", "fuzzers", timeout=40 * 60)
    await ninja_cmd(build)
    await ninja_cmd(build, *args, timeout=None)


if __name__ == "__main__":
    with main():
        run(ninja(*argv[1:]))
