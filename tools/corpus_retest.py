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
from itertools import product
from os import environ
from pathlib import Path
from re import escape, finditer
from sys import argv
from uuid import uuid4

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, chunks, cmd_flog, main
from corpus_utils import corpus_merge, corpus_trim, fuzz_star, regression
from ninja import ninja
from structlog import get_logger

IN_CI = environ.get("CI", "") == "true"
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CORPUS_ORIGINAL = FUZZ / "corpus_original"


def fuzz_output_paths(prefix: bytes, output: bytes) -> set[bytes]:
    return set(
        map(
            lambda m: m["path"], finditer(escape(prefix) + rb"\s+(?P<path>\S+)", output)
        )
    )


async def regression_log_one(fuzzer: Path, chunk: list[Path]) -> Exception | None:
    log_file = f"/tmp/{fuzzer.name}-{uuid4().hex}.log"
    Path(log_file).write_bytes(b"")
    try:
        await cmd_flog(fuzzer, *chunk, out=log_file, timeout=None)
    except ProcessError as err:
        return err
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
        Path(log_file).unlink(missing_ok=True)
    return None


async def regression_log() -> list[Exception]:
    return list(
        filter(
            None,
            await as_completed(
                map(
                    lambda args: regression_log_one(*args),
                    product(
                        fuzz_star(),
                        chunks(filter(Path.is_file, CORPUS.rglob("*")), 4096),
                    ),
                )
            ),
        )
    )


def rmdir(path: Path) -> None:
    if path.is_dir():
        for child in path.iterdir():
            rmdir(child)
        path.rmdir()
    else:
        path.unlink(missing_ok=True)


async def corpus_retest(build: str) -> None:
    await ninja(build)
    while await regression_log():
        get_logger().info(
            "Regression failed, retrying with"
            f" {len(list(filter(Path.is_file, CORPUS.rglob('*'))))}"
        )
    if IN_CI:
        rmdir(CORPUS_ORIGINAL)
    CORPUS.rename(CORPUS_ORIGINAL)
    CORPUS.mkdir(parents=True, exist_ok=True)
    errors = await corpus_merge(CORPUS_ORIGINAL)
    if errors:
        error = errors.pop()
        if errors:
            for error in errors:
                get_logger().error(f"Extra Error: {error}")
        raise error


async def corpus_retest_main(*args: str, disable_bars: bool) -> None:
    corpus_trim(disable_bars=disable_bars)
    await corpus_retest(*args)
    await regression(*args)


if __name__ == "__main__":
    with main():
        run(corpus_retest_main(*argv[1:], disable_bars=False))
