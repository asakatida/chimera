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

from hashlib import sha256
from itertools import chain, repeat
from pathlib import Path
from re import MULTILINE, compile
from sys import stderr
from typing import Iterable

from corpus_utils import bucket, c_tqdm, gather_paths, sha

LENGTH = 14
DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"
CONFLICT = compile(rb"^(?:(?:<{7,8}|>{7,8})(?:\s.+)?|={7,8})$\s?", MULTILINE)


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


def conflicts(fuzz: Iterable[Path], disable_bars: bool) -> None:
    set(
        c_tqdm(
            map(
                conflicts_one,
                filter(lambda file: CONFLICT.search(file.read_bytes()), fuzz),
            ),
            "Conflicts",
            disable_bars,
        )
    )


def corpus_trim_one(fuzz: Iterable[Path], disable_bars: bool) -> None:
    global LENGTH
    for file in c_tqdm(fuzz, "Corpus rehash", disable_bars):
        src_sha = sha(file)
        name = src_sha[:LENGTH]
        if name == (file.parent.name + file.name):
            continue
        sha_bucket, name = name[:2], name[2:]
        if set(
            map(
                sha,
                filter(
                    Path.exists,
                    map(FUZZ.joinpath, DIRECTORIES, repeat(sha_bucket), repeat(name)),
                ),
            )
        ).difference({src_sha}):
            LENGTH += 1
            raise Increment(
                f"Collision found, update corpus_trim.py `LENGTH`: {LENGTH}"
            )
        new_file = bucket(file) / sha_bucket / name
        new_file.parent.mkdir(parents=True, exist_ok=True)
        file.rename(new_file)
    for file in filter(
        Path.exists,
        map(
            CRASHES.joinpath,
            map(
                lambda path: path.relative_to(bucket(path)),
                filter(
                    Path.is_file,
                    c_tqdm(CORPUS.rglob("*"), "Regression trim", disable_bars),
                ),
            ),
        ),
    ):
        file.unlink()


def corpus_trim(disable_bars: bool = False) -> None:
    conflicts(gather_paths(), disable_bars)
    CRASHES.mkdir(parents=True, exist_ok=True)
    for file in c_tqdm(
        chain.from_iterable(map(SOURCE.rglob, ("crash-*", "leak-*", "timeout-*"))),
        "Crashes gather",
        disable_bars,
    ):
        file.rename(CRASHES / sha(file))
    while True:
        try:
            corpus_trim_one(gather_paths(), disable_bars)
        except Increment:
            if LENGTH > 32:
                raise
            continue
        break


if __name__ == "__main__":
    try:
        corpus_trim()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
