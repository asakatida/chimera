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

from asyncio import run
from hashlib import sha256
from itertools import chain, repeat
from operator import attrgetter
from pathlib import Path
from re import MULTILINE, compile
from sys import stderr
from typing import Iterable, TypeVar

from asyncio_cmd import ProcessError
from corpus_utils import c_tqdm, fuzz_star, fuzz_test, gather_paths

LENGTH = 8
DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CORPUS_ORIGINAL = FUZZ / "corpus_original"
CRASHES = FUZZ / "crashes"
T = TypeVar("T")
CONFLICT = compile(rb"^((<{8}|>{8})\s.+|={8})$\s", MULTILINE)


class Increment(Exception):
    pass


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
    set(
        c_tqdm(
            map(
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
        if name == file.name:
            continue
        if set(
            map(
                sha,  # type: ignore
                filter(
                    Path.exists,  # type: ignore
                    map(Path.joinpath, map(FUZZ.joinpath, DIRECTORIES), repeat(name)),  # type: ignore
                ),
            )
        ).difference({src_sha}):
            LENGTH += 1
            raise Increment(
                f"Collision found, update corpus_trim.py `LENGTH`: {LENGTH}"
            )
        file.rename(file.parent / name)
    for file in filter(
        Path.exists,
        map(
            CORPUS.joinpath,
            map(attrgetter("name"), c_tqdm(CRASHES.iterdir(), "Regression trim")),
        ),
    ):
        file.unlink()


def corpus_trim() -> None:
    while True:
        try:
            corpus_trim_one(gather_paths())
        except Increment:
            if LENGTH > 32:
                raise
            continue
        break


def sha(path: Path) -> str:
    return sha256(path.read_bytes()).hexdigest()


async def main() -> None:
    if not fuzz_star():
        raise FileNotFoundError("No fuzz targets built")
    CORPUS.mkdir(exist_ok=True)
    CRASHES.mkdir(exist_ok=True)
    conflicts(gather_paths())
    corpus_trim()
    CORPUS.rename(CORPUS_ORIGINAL)
    CORPUS.mkdir(exist_ok=True)
    errors = await fuzz_test(
        "-merge=1",
        "-reduce_inputs=1",
        "-shrink=1",
        CORPUS,
        CORPUS_ORIGINAL,
    )
    if errors:
        error = errors.pop()
        if errors:
            print("Extra Errors:", *errors, file=stderr, sep="\n")
        raise error
    for file in chain(
        SOURCE.rglob("crash-*"), SOURCE.rglob("leak-*"), SOURCE.rglob("timeout-*")
    ):
        file.rename(CRASHES / sha(file))
    corpus_trim()


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
