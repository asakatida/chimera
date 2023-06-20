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
from typing import Iterable

from asyncio_cmd import main
from corpus_utils import bucket, gather_paths
from structlog import get_logger

PRINTABLE = set(printable)


def is_ascii(data: bytes) -> bool:
    try:
        return all(c in PRINTABLE or ord(c) > 0xFF for c in data.decode())
    except UnicodeDecodeError:
        return False


def _is_ascii(path: Path) -> bool:
    return is_ascii(path.read_bytes())


def corpus_ascii() -> Iterable[Path]:
    return filter(_is_ascii, gather_paths())


if __name__ == "__main__":
    with main():
        paths = sorted(corpus_ascii())
        for file in paths:
            get_logger().info(file)
        for name, contents in groupby(paths, lambda file: bucket(file).name):
            total = len(tuple(contents))
            get_logger().info(f"{name}: {total} / {total * 100 // len(paths) / 100}")
