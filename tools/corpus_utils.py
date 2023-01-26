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

from functools import cache
from itertools import chain, repeat
from pathlib import Path
from typing import Iterable, TypeVar

from asyncio_as_completed import a_list, as_completed
from asyncio_cmd import cmd_check
from tqdm import tqdm  # type: ignore

DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"
T = TypeVar("T")


def c_tqdm(iterable: T, desc: str, total: int = 0) -> T:
    return tqdm(iterable, desc=desc, maxinterval=60, miniters=100, total=total, unit_scale=True)  # type: ignore


@cache
def fuzz_star() -> tuple[Path, ...]:
    return tuple(
        filter(
            lambda path: path.stat().st_mode & 0o110,
            filter(Path.is_file, SOURCE.rglob("fuzz-*")),
        )
    )


async def fuzz_test(*args: object, timeout: int = 240) -> list[Exception]:
    return list(
        filter(
            None,
            await a_list(
                as_completed(
                    map(
                        lambda *args: cmd_check(*args, timeout=timeout),
                        fuzz_star(),
                        *map(repeat, args)  # type: ignore
                    )
                )
            ),
        )
    )


def gather_paths() -> Iterable[Path]:
    return chain.from_iterable(map(Path.iterdir, map(FUZZ.joinpath, DIRECTORIES)))
