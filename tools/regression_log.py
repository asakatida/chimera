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

"""regression_log.py"""

from asyncio import run
from pathlib import Path
from re import escape, finditer
from sys import argv, stderr

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, chunks, cmd_flog


def fuzz_output_paths(prefix: bytes, output: bytes) -> set[bytes]:
    return set(
        map(
            lambda m: m["path"],
            finditer(
                escape(prefix) + rb"\s+(?P<path>\S+/fuzz/corpus/[0-9a-f]{2}/[0-9a-f]+)",
                output,
            ),
        )
    )


async def regression(fuzzer: str, corpus: str) -> None:
    log_file = f"/tmp/{fuzzer}.log"
    Path(log_file).write_bytes(b"")
    try:
        await as_completed(
            map(
                lambda args: cmd_flog(
                    f"./fuzz-{fuzzer}", *args, out=log_file, timeout=1200
                ),
                chunks(filter(Path.is_file, Path(corpus).rglob("*")), 512),
            ),
            limit=5,
        )
    finally:
        log_output = Path(log_file).read_bytes()
        for file in map(
            Path,
            map(
                bytes.decode,
                fuzz_output_paths(b"Running:", log_output)
                - fuzz_output_paths(b"Executed", log_output),
            ),
        ):
            file.unlink(missing_ok=True)


if __name__ == "__main__":
    try:
        run(regression(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
