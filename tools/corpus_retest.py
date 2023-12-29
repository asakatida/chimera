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

"""corpus_retest.py."""

from asyncio import run
from pathlib import Path
from re import escape, finditer
from sys import argv
from uuid import uuid4

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, chunks, cmd_flog, main
from cmake_codecov import cmake_codecov
from corpus_utils import corpus_merge, corpus_trim, fuzz_star, regression
from structlog import get_logger

SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"


def fuzz_output_paths(prefix: bytes, output: bytes) -> set[bytes]:
    return {m["path"] for m in finditer(escape(prefix) + rb"\s+(?P<path>\S+)", output)}


async def regression_log_one(fuzzer: Path, *chunk: Path) -> Exception | None:
    log_file = f"/tmp/{fuzzer.name}-{uuid4().hex}.log"
    Path(log_file).write_bytes(b"")
    try:
        await cmd_flog(fuzzer, *chunk, out=log_file)
    except ProcessError as err:
        return err
    finally:
        log_output = Path(log_file).read_bytes()
        for file in (
            Path(path.decode())
            for path in fuzz_output_paths(b"Running:", log_output)
            - fuzz_output_paths(b"Executed", log_output)
        ):
            try:
                file.rename(CRASHES / file.name)
            except FileNotFoundError:
                pass
        Path(log_file).unlink(missing_ok=True)
    return None


async def regression_log() -> list[Exception]:
    return [
        exc
        for exc in await as_completed(
            regression_log_one(fuzz, *args)
            for args in chunks(
                (path for path in CORPUS.rglob("*") if path.is_file()), 64
            )
            for fuzz in fuzz_star()
        )
        if exc is not None
    ]


async def corpus_retest() -> None:
    CRASHES.mkdir(exist_ok=True, parents=True)
    while await regression_log():
        await get_logger().ainfo(
            "Regression failed, retrying with"
            f" {len([path for path in CORPUS.rglob('*') if path.is_file()])} cases"
        )
    await corpus_merge(disable_bars=None)


async def corpus_retest_main(build: str) -> None:
    await cmake_codecov("fuzzers")
    await corpus_retest()
    await regression(build)
    corpus_trim(disable_bars=None)


if __name__ == "__main__":
    with main():
        run(corpus_retest_main(*argv[1:]))
