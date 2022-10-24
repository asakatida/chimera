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
from re import MULTILINE, compile
from subprocess import DEVNULL, PIPE, CalledProcessError, TimeoutExpired, run
from sys import stderr
from typing import Iterable, Optional, TypeVar

from tqdm import tqdm  # type: ignore

LENGTH = 22
DIRECTORIES = ("corpus", "crashes")
FUZZ = Path("unit_tests") / "fuzz"
CORPUS = FUZZ / "corpus"
CORPUS_ORIGINAL = FUZZ / "corpus_original"
CRASHES = FUZZ / "crashes"
T = TypeVar("T")
CONFLICT = compile(rb"^((<{8}|>{8})\s.+|={8})$\s", MULTILINE)


class Increment(Exception):
    pass


def c_tqdm(iterable: Iterable[T], desc: str) -> Iterable[T]:
    return tqdm(iterable, desc=desc, maxinterval=60, mininterval=10, miniters=100, unit_scale=True)  # type: ignore


def conflicts_one(file: Path) -> None:
    set(
        map(
            lambda section: (file.parent / sha256(section).hexdigest()).write_bytes(
                section
            ),
            filter(None, CONFLICT.split(file.read_bytes())),
        )
    )
    file.unlink()


def conflicts(fuzz: Iterable[Path]) -> None:
    with ThreadPoolExecutor() as executor:
        set(
            c_tqdm(
                executor.map(
                    conflicts_one,
                    filter(
                        lambda file: CONFLICT.search(file.read_bytes()),
                        fuzz,
                    ),
                ),
                "Conflicts",
            )
        )


def corpus_trim_one(fuzz: Iterable[Path]) -> None:
    global LENGTH
    for file in c_tqdm(fuzz, "Corpus rehash"):
        src_sha = sha(file)
        name = src_sha[:LENGTH]
        if any(
            map(
                lambda other: other.exists() and src_sha != sha(other),
                map(lambda directory: FUZZ / directory / name, DIRECTORIES),
            )
        ):
            LENGTH += 1
            raise Increment(
                f"Collision found, update corpus_trim.py `LENGTH`: {LENGTH}"
            )
        file.rename(file.parent / name)


def corpus_trim() -> None:
    while True:
        try:
            corpus_trim_one(gather())
        except Increment:
            if LENGTH > 32:
                raise
            continue
        break


@cache
def fuzz_star() -> tuple[Path, ...]:
    return tuple(
        filter(
            lambda path: path.is_file() and path.stat().st_mode & 0o110,
            Path().rglob("fuzz-*"),
        )
    )


def fuzz_test_one(regression: Path, *args: str, timeout: int) -> Optional[Exception]:
    try:
        run(
            [str(regression), "-detect_leaks=0", "-use_value_profile=1", *args],
            check=True,
            stderr=PIPE,
            stdout=DEVNULL,
            timeout=timeout,
        )
    except CalledProcessError as error:
        return error
    except TimeoutExpired as error:
        return error
    return None


def fuzz_test(*args: str, timeout: int = 10) -> list[Exception]:
    with ThreadPoolExecutor() as executor:
        return list(
            filter(
                None,
                executor.map(
                    lambda regression: fuzz_test_one(
                        regression, *args, timeout=timeout
                    ),
                    fuzz_star(),
                ),
            )
        )


def gather() -> Iterable[Path]:
    return chain.from_iterable(
        map(lambda directory: (FUZZ / directory).iterdir(), DIRECTORIES)
    )


def sha(path: Path) -> str:
    return sha256(path.read_bytes()).hexdigest()


def regression_one(file: Path) -> None:
    if not fuzz_test(str(file)):
        file.rename(CORPUS / file.name)


def regression(fuzz: Iterable[Path]) -> None:
    with ThreadPoolExecutor() as executor:
        set(c_tqdm(executor.map(regression_one, fuzz), "Regression"))


def main() -> None:
    chdir(Path(__file__).parent.parent)
    if not fuzz_star():
        raise FileNotFoundError("No fuzz targets built")
    CORPUS.mkdir(exist_ok=True)
    CRASHES.mkdir(exist_ok=True)
    conflicts(gather())
    corpus_trim()
    regression(CRASHES.iterdir())
    CORPUS.rename(CORPUS_ORIGINAL)
    for _ in range(2):
        CORPUS.mkdir(exist_ok=True)
        errors = fuzz_test(
            "-merge=1",
            "-reduce_inputs=1",
            "-shrink=1",
            str(CORPUS),
            str(CORPUS_ORIGINAL),
            timeout=1200,
        )
        if not errors:
            break
    if errors:
        error = errors.pop()
        if errors:
            print("Extra Errors:", *errors, file=stderr, sep="\n")
        raise error
    for file in chain(
        Path().rglob("crash-*"), Path().rglob("leak-*"), Path().rglob("timeout-*")
    ):
        file.rename(CRASHES / sha(file))
    corpus_trim()


if __name__ == "__main__":
    try:
        main()
    except CalledProcessError as error:
        print(*error.cmd, file=stderr)
        print((error.stderr or b"").decode(), file=stderr)
        exit(error.returncode)
    except TimeoutExpired as error:
        print(*error.cmd, file=stderr)
        print((error.stderr or b"").decode(), file=stderr)
        exit(1)
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
