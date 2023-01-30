# Copyright (c) 2017 Asa Katida <github@holomaplefeline.net>
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

"""generate_utf8_id_start.py."""

from itertools import chain, count, groupby
from pathlib import Path
from re import MULTILINE, subn
from typing import Iterable, Tuple

from asyncio_cmd import chunks
from tqdm import tqdm

utf8_id_start = (
    Path(__file__).parent.parent / "library" / "grammar" / "utf8_id_start.hpp"
).resolve()


def _slices(total: int, it: Iterable[int]) -> Iterable[Iterable[str]]:
    return chunks(tqdm(map(hex, it), total=total), 64)


def _ranges(total: int, it: Iterable[int]) -> str:
    ranges = ">, ranges<".join(map(",".join, _slices(total, it)))
    return f"sor<ranges<{ranges}>>"


def _a(t: tuple[int, Iterable[Tuple[int, int]]]) -> Tuple[int, ...]:
    groups = tuple(e[0] for e in t[1])
    return (min(groups), max(groups))


ranges = _ranges(
    1334,
    chain.from_iterable(
        map(
            _a,
            groupby(
                zip(
                    map(ord, filter(str.isidentifier, map(chr, range(0x10FFFF)))),
                    count(),
                ),
                lambda t: t[0] - t[1],
            ),
        )
    ),
)

utf8_id_start.write_text(
    subn(
        r"\bUtf8IdStart\b[^;]+",
        f"Utf8IdStart = {ranges}",
        utf8_id_start.read_text(),
        1,
        MULTILINE,
    )[0]
)
