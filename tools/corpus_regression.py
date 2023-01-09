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
from typing import Iterable, TypeVar

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError
from corpus_utils import c_tqdm, fuzz_star, fuzz_test, gather_paths

DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"
T = TypeVar("T")


async def regression_one(file: Path) -> None:
    if file == CRASHES / file.name:
        if await fuzz_test(file):
            file.rename(CORPUS / file.name)
    elif not await fuzz_test(file):
        file.rename(CRASHES / file.name)


async def regression(fuzz: Iterable[Path]) -> None:
    await as_completed(c_tqdm(map(regression_one, fuzz), "Regression"))


async def main() -> None:
    if not fuzz_star():
        raise FileNotFoundError("No fuzz targets built")
    CORPUS.mkdir(exist_ok=True)
    CRASHES.mkdir(exist_ok=True)
    await regression(sample(list(gather_paths()), 1_000))


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
