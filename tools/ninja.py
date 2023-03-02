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
from asyncio.subprocess import PIPE
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd, cmd_no_timeout


async def ninja_cmd(*args: object, timeout: int) -> None:
    await cmd("ninja", "-C", *args, err=None, out=None, timeout=timeout)


async def ninja(build: object, *args: object) -> None:
    targets = await cmd("ninja", "-C", build, "help", err=None, out=PIPE)
    await ninja_cmd(build, "-j1", "chimera-grammar", timeout=2400)
    await ninja_cmd(build, "-j3", "chimera", "chimera-core", "libchimera", timeout=600)
    if b"fuzzers: phony" in targets.splitlines():
        await ninja_cmd(build, "-j1", "fuzzers", timeout=2400)
    await ninja_cmd(build, "Catch2", timeout=180)
    await ninja_cmd(build, "-j1", "unit-test", timeout=2400)
    await ninja_cmd(build, timeout=600)
    await cmd_no_timeout("ninja", "-C", build, "-j3", *args)


if __name__ == "__main__":
    try:
        run(ninja(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
