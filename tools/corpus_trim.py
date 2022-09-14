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
from pathlib import Path

LENGTH = 9
DIRECTORIES = ("corpus", "crashes")


class Increment(Exception):
    pass


def sha(path: Path) -> str:
    return sha256(path.read_bytes()).hexdigest()


def corpus_trim(corpus: Path) -> None:
    global LENGTH
    for file in corpus.glob("*/*"):
        if file.parent.name not in DIRECTORIES:
            continue
        name = file.name[:LENGTH].rjust(LENGTH, "0")
        if any(
            map(
                lambda other: other.exists() and sha(file) != sha(other),
                map(lambda directory: corpus / directory / name, DIRECTORIES),
            )
        ):
            raise Increment("Collision found, update corpus_trim.py `LENGTH`:", LENGTH)
        file.rename(file.parent / name)


def main() -> None:
    global LENGTH
    corpus = (Path(__file__).parent.parent / "unit_tests" / "fuzz").absolute()
    while True:
        try:
            corpus_trim(corpus)
        except Increment:
            LENGTH += 1
            continue
        break


if __name__ == "__main__":
    main()
