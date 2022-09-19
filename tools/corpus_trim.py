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

"""corpus_trim.py"""

from concurrent.futures import ThreadPoolExecutor
from functools import cache
from hashlib import sha256
from itertools import chain
from os import chdir
from pathlib import Path
from subprocess import DEVNULL, CalledProcessError, run
from typing import Iterator, TypeVar

from tqdm import tqdm  # type: ignore

LENGTH = 12
DIRECTORIES = ("corpus", "crashes")
FUZZ = Path(__file__).absolute().parent.parent / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CORPUS_ORIGINAL = FUZZ / "corpus_original"
CRASHES = FUZZ / "crashes"
T = TypeVar("T")


class Increment(Exception):
    pass


def c_tqdm(iterable: Iterator[T], desc: str, total: int) -> Iterator[T]:
    return tqdm(iterable, delay=5, desc=desc, maxinterval=60, mininterval=10, miniters=100, total=total, unit_scale=True)  # type: ignore


def corpus_trim(fuzz: Path, total: int) -> None:
    global LENGTH
    for file in c_tqdm(fuzz.glob("*/*"), "Corpus rehash", total):
        if file.parent.name not in DIRECTORIES:
            continue
        src_sha = sha(file)
        name = src_sha[:LENGTH]
        if any(
            map(
                lambda other: other.exists() and src_sha != sha(other),
                map(lambda directory: fuzz / directory / name, DIRECTORIES),
            )
        ):
            raise Increment("Collision found, update corpus_trim.py `LENGTH`:", LENGTH)
        file.rename(file.parent / name)


@cache
def fuzz_star() -> bytes:
    return b"\0".join(
        map(
            lambda path: str(path).encode(),
            filter(
                lambda path: path.is_file() and path.stat().st_mode & 0o110,
                Path().rglob("fuzz-*"),
            ),
        )
    )


def fuzz_test(*args: str, timeout: int = 10) -> None:
    run(
        [
            "xargs",
            "--null",
            "--replace={}",
            "--",
            "env",
            "{}",
            "-detect_leaks=0",
            "-use_value_profile=1",
            *args,
        ],
        check=True,
        input=fuzz_star(),
        stderr=DEVNULL,
        stdout=DEVNULL,
        timeout=timeout,
    )


def sha(path: Path) -> str:
    return sha256(path.read_bytes()).hexdigest()


def regression_one(file: Path) -> bool:
    if file.parent.name not in DIRECTORIES:
        return False
    try:
        fuzz_test(str(file))
    except CalledProcessError:
        if file.is_relative_to(CORPUS):
            file.rename(CRASHES / file.name)
            return True
    if file.is_relative_to(CRASHES):
        file.rename(CORPUS / file.name)
        return True
    return False


def regression(fuzz: Path, total: int) -> bool:
    with ThreadPoolExecutor() as executor:
        return any(
            set(
                c_tqdm(
                    executor.map(regression_one, fuzz.glob("*/*")),
                    "Regression",
                    total,
                )
            )
        )


def main() -> None:
    global LENGTH
    chdir(Path(__file__).parent.parent)
    if not fuzz_star():
        raise FileNotFoundError("No fuzz targets built")
    CORPUS.mkdir(exist_ok=True)
    CRASHES.mkdir(exist_ok=True)
    total = len(set(FUZZ.glob("*/*")))
    if not regression(FUZZ, total):
        CORPUS.rename(CORPUS_ORIGINAL)
        run(
            ["git", "restore", "--source", "origin/HEAD", str(CORPUS)],
            check=True,
            stderr=DEVNULL,
            stdout=DEVNULL,
            timeout=10,
        )
        fuzz_test(
            "-merge=1",
            "-reduce_inputs=1",
            "-shrink=1",
            str(CORPUS),
            str(CORPUS_ORIGINAL),
            timeout=600,
        )
    for file in chain(
        Path().rglob("crash-*"), Path().rglob("leak-*"), Path().rglob("timeout-*")
    ):
        file.rename(CRASHES / sha(file))
    while True:
        try:
            corpus_trim(FUZZ, total)
        except Increment:
            LENGTH += 1
            continue
        break


if __name__ == "__main__":
    main()
