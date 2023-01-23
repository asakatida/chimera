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
from pathlib import Path
from random import sample
from sys import stderr
from typing import Iterable

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError
from corpus_utils import c_tqdm, fuzz_star, fuzz_test, gather_paths

DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CORPUS_ORIGINAL = FUZZ / "corpus_original"
CRASHES = FUZZ / "crashes"


async def regression_one(file: Path) -> None:
    if file.is_relative_to(CRASHES):
        if await fuzz_test(file):
            file.rename(CORPUS_ORIGINAL / file.name)
            file = CORPUS_ORIGINAL / file.name
            if await fuzz_test(
                "-merge=1", "-reduce_inputs=1", "-shrink=1", CORPUS, CORPUS_ORIGINAL
            ):
                pass
            else:
                file.rename(CRASHES / file.name)
    elif not await fuzz_test(file):
        file.rename(CRASHES / file.name)


async def regression(fuzz: Iterable[Path]) -> None:
    async for _ in as_completed(c_tqdm(map(regression_one, fuzz), "Regression", 1_000)):
        pass


async def main() -> None:
    if not fuzz_star():
        raise FileNotFoundError("No fuzz targets built")
    CORPUS.mkdir(exist_ok=True)
    CORPUS_ORIGINAL.mkdir(exist_ok=True)
    CRASHES.mkdir(exist_ok=True)
    await regression(sample(list(gather_paths()), 1_000))


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
