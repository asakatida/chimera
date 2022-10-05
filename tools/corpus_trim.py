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
from typing import Iterable, TypeVar

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


def conflicts(fuzz: list[Path]) -> None:
    with ThreadPoolExecutor() as executor:
        set(
            executor.map(
                conflicts_one,
                filter(
                    lambda file: CONFLICT.search(file.read_bytes()),
                    c_tqdm(fuzz, "Conflicts"),
                ),
            )
        )


def corpus_trim(fuzz: list[Path]) -> None:
    global LENGTH
    for file in c_tqdm(fuzz, "Corpus rehash"):
        if not (file.exists() and file.parent.name in DIRECTORIES):
            continue
        src_sha = sha(file)
        name = src_sha[:LENGTH]
        if any(
            map(
                lambda other: other.exists() and src_sha != sha(other),
                map(lambda directory: FUZZ / directory / name, DIRECTORIES),
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
    if not (file.exists() and file.parent.name in DIRECTORIES):
        return False
    try:
        fuzz_test(str(file))
    except CalledProcessError:
        if file.is_relative_to(CORPUS):
            file.rename(CRASHES / file.name)
            return True
    except TimeoutExpired:
        if file.is_relative_to(CORPUS):
            file.rename(CRASHES / file.name)
            return True
    if file.is_relative_to(CRASHES):
        file.rename(CORPUS / file.name)
        return True
    return False


def regression(fuzz: list[Path]) -> bool:
    with ThreadPoolExecutor() as executor:
        return any(
            set(
                c_tqdm(
                    executor.map(regression_one, fuzz),
                    "Regression",
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
    fuzz = sorted(tqdm(FUZZ.glob("*/*"), desc="Gather", unit_scale=True))
    conflicts(fuzz)
    if not regression(fuzz):
        CORPUS.rename(CORPUS_ORIGINAL)
        run(
            ["git", "restore", "--source", "origin/HEAD", "--worktree", str(CORPUS)],
            check=True,
            stderr=PIPE,
            stdout=DEVNULL,
            timeout=10,
        )
        fuzz_test(
            "-merge=1",
            "-reduce_inputs=1",
            "-shrink=1",
            str(CORPUS),
            str(CORPUS_ORIGINAL),
            timeout=1200,
        )
    for file in chain(
        Path().rglob("crash-*"), Path().rglob("leak-*"), Path().rglob("timeout-*")
    ):
        file.rename(CRASHES / sha(file))
    while True:
        try:
            corpus_trim(fuzz)
        except Increment:
            LENGTH += 1
            if LENGTH > 32:
                raise
            continue
        break


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
        print()
