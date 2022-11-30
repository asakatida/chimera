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

"""test.py"""

from hashlib import sha256
from operator import attrgetter
from pathlib import Path
from sys import stderr
from typing import Iterable, TypeVar

from corpus_utils import gather_paths
from tqdm import tqdm  # type: ignore

LENGTH = 8
DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"
T = TypeVar("T")


class Increment(Exception):
    pass


def c_tqdm(iterable: Iterable[T], desc: str) -> Iterable[T]:
    return tqdm(iterable, desc=desc, maxinterval=60, miniters=100, unit_scale=True)  # type: ignore


def corpus_trim_one(fuzz: Iterable[Path]) -> None:
    global LENGTH
    for file in c_tqdm(fuzz, "Corpus rehash"):
        src_sha = sha(file)
        name = src_sha[:LENGTH]
        if name == file.name:
            continue
        if src_sha in map(
            sha,
            filter(
                Path.exists,
                map(Path.joinpath, map(FUZZ.joinpath, DIRECTORIES), (name,) * 2),
            ),
        ):
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


def main() -> None:
    CORPUS.mkdir(exist_ok=True)
    CRASHES.mkdir(exist_ok=True)
    corpus_trim()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
