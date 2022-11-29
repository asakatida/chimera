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

"""corpus_regression.py"""

from asyncio import run
from asyncio.subprocess import DEVNULL
from functools import cache
from itertools import chain
from pathlib import Path
from sys import stderr
from typing import AsyncGenerator, Iterable, Optional, TypeVar

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, cmd
from tqdm.asyncio import tqdm  # type: ignore

DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"
T = TypeVar("T")


def c_atqdm(
    iterable: AsyncGenerator[T, object], desc: str
) -> AsyncGenerator[T, object]:
    return tqdm(iterable, desc=desc, maxinterval=60, miniters=100, unit_scale=True)  # type: ignore


@cache
def fuzz_star() -> tuple[Path, ...]:
    return tuple(
        filter(
            lambda path: path.is_file() and path.stat().st_mode & 0o110,
            SOURCE.rglob("fuzz-*"),
        )
    )


async def fuzz_test_one(
    regression: Path, *args: str, stdout: Optional[int], timeout: int
) -> Optional[Exception]:
    try:
        await cmd(
            str(regression),
            "-detect_leaks=0",
            "-use_value_profile=1",
            *args,
            log=False,
            stdout=stdout,
            timeout=timeout,
        )
    except Exception as error:
        return error
    return None


async def fuzz_test(
    *args: str, stdout: Optional[int] = DEVNULL, timeout: int = 10
) -> list[Exception]:
    results = []
    async for error in as_completed(
        map(
            lambda regression: fuzz_test_one(
                regression, *args, stdout=stdout, timeout=timeout
            ),
            fuzz_star(),
        ),
        limit=12,
    ):
        if error is not None:
            results.append(error)
    return results


def gather_paths() -> Iterable[Path]:
    return chain.from_iterable(
        map(lambda directory: (FUZZ / directory).iterdir(), DIRECTORIES)
    )


async def regression_one(file: Path) -> None:
    if file == CRASHES / file.name:
        if await fuzz_test(str(file)):
            file.rename(CORPUS / file.name)
    elif not await fuzz_test(str(file)):
        file.rename(CRASHES / file.name)


async def regression(fuzz: Iterable[Path]) -> None:
    async for _ in c_atqdm(
        as_completed(map(regression_one, fuzz), limit=12), "Regression"
    ):
        pass


async def main() -> None:
    if not fuzz_star():
        raise FileNotFoundError("No fuzz targets built")
    CORPUS.mkdir(exist_ok=True)
    CRASHES.mkdir(exist_ok=True)
    await regression(gather_paths())


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
