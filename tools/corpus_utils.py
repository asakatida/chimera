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
from hashlib import sha256
from itertools import chain, repeat
from pathlib import Path
from typing import Iterable, TypeVar

from asyncio_as_completed import as_completed
from asyncio_cmd import cmd_check
from tqdm import tqdm

DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
T = TypeVar("T")
CORPUS.mkdir(parents=True, exist_ok=True)


def c_tqdm(iterable: Iterable[T], desc: str, total: int = 0) -> Iterable[T]:
    return tqdm(
        iterable, desc=desc, maxinterval=60, miniters=100, total=total, unit_scale=True
    )


async def corpus_merge(path: Path) -> list[Exception]:
    return await fuzz_test(
        "-merge=1",
        "-reduce_inputs=1",
        "-shrink=1",
        CORPUS,
        *filter(
            Path.is_dir,
            chain.from_iterable(map(lambda path: path.rglob("*"), (CORPUS, path))),
        ),
        timeout=3600
    )


def bucket(path: Path) -> Path:
    return FUZZ / path.relative_to(FUZZ).parts[0]


@cache
def fuzz_star() -> tuple[Path, ...]:
    return tuple(
        filter(
            lambda path: path.stat().st_mode & 0o110,
            filter(Path.is_file, SOURCE.rglob("fuzz-*")),
        )
    )


async def fuzz_test(*args: object, timeout: int = 240) -> list[Exception]:
    if not fuzz_star():
        raise FileNotFoundError("No fuzz targets built")
    return list(
        filter(
            None,
            await as_completed(
                map(
                    lambda cmd, args: cmd_check(cmd, *args, timeout=timeout),
                    fuzz_star(),
                    repeat(args),
                )
            ),
        )
    )


def gather_paths() -> Iterable[Path]:
    return filter(
        Path.is_file,
        chain.from_iterable(
            map(Path.rglob, map(FUZZ.joinpath, DIRECTORIES), repeat("*"))  # type: ignore
        ),
    )


def sha(path: Path) -> str:
    return sha256(path.read_bytes()).hexdigest()
