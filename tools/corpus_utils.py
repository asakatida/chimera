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

"""corpus_utils.py"""

from asyncio.subprocess import DEVNULL
from functools import cache, partial
from itertools import chain, repeat
from pathlib import Path
from typing import Iterable, Optional, TypeVar

from asyncio_as_completed import as_completed
from asyncio_cmd import cmd
from tqdm import tqdm  # type: ignore

DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"
T = TypeVar("T")


def c_tqdm(iterable: Iterable[T], desc: str) -> Iterable[T]:
    return tqdm(iterable, desc=desc, maxinterval=60, miniters=100, unit_scale=True)  # type: ignore


@cache
def fuzz_star() -> tuple[Path, ...]:
    return tuple(
        filter(
            lambda path: path.stat().st_mode & 0o110,
            filter(Path.is_file, SOURCE.rglob("fuzz-*")),
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
    return list(
        filter(
            None,
            await as_completed(
                map(
                    partial(fuzz_test_one, stdout=stdout, timeout=timeout),
                    fuzz_star(),
                    *map(repeat, args),  # type: ignore
                ),
                limit=12,
            ),
        )
    )


def gather_paths() -> Iterable[Path]:
    return chain(*map(Path.iterdir, map(FUZZ.joinpath, DIRECTORIES)))  # type: ignore
