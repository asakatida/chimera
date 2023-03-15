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

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError
from corpus_utils import c_tqdm, corpus_merge, fuzz_test, gather_paths

DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"


async def regression_one(file: Path) -> None:
    if file.is_relative_to(CRASHES):
        if await fuzz_test(file):
            await corpus_merge(file)
    elif not await fuzz_test(file):
        file.rename(CRASHES / file.relative_to(CORPUS))


async def corpus_regression() -> None:
    await as_completed(
        c_tqdm(
            map(regression_one, sample(list(gather_paths()), 1_000)),
            "Regression",
            1_000,
        )
    )


if __name__ == "__main__":
    try:
        run(corpus_regression())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
