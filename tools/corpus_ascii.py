# Copyright (c) 2023 Asa Katida <github@holomaplefeline.net>
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

"""corpus_ascii.py"""

from itertools import groupby
from pathlib import Path
from string import printable
from sys import stderr
from typing import Iterable

from corpus_utils import gather_paths

FUZZ = Path(__file__).parent.parent.resolve() / "unit_tests" / "fuzz"
PRINTABLE = set(printable.encode())


def corpus_ascii() -> Iterable[Path]:
    return filter(lambda file: set(file.read_bytes()) <= PRINTABLE, gather_paths())


if __name__ == "__main__":
    try:
        paths = sorted(corpus_ascii())
        for file in paths:
            print(file)
        for name, contents in groupby(
            paths, lambda file: file.relative_to(FUZZ).parts[0]
        ):
            total = len(tuple(contents))
            print(name, total, total * 100 // len(paths) / 100, file=stderr)
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
