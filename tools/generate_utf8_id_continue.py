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

"""generate_utf8_id_continue.py."""

from itertools import chain, count, groupby, islice, repeat, starmap, takewhile
from pathlib import Path
from re import MULTILINE, subn
from typing import Iterator, Set, Tuple

from tqdm import tqdm  # type: ignore

utf8_id_continue = (
    Path(__file__).parent.parent / "library" / "grammar" / "utf8_id_continue.hpp"
).absolute()


def _slices(total: int, it: Iterator[int]) -> Iterator[Iterator[str]]:
    return map(islice, repeat(iter(tqdm(map(hex, it), total=total))), repeat(64))


def _ranges(total: int, it: Iterator[int]) -> str:
    ranges = ">, ranges<".join(
        takewhile(
            lambda r: r,
            map(",".join, _slices(total, it)),
        )
    )
    return f"sor<ranges<{ranges}>>"


def _a(id_start: Set[int]) -> Iterator[int]:
    def b(i: int) -> bool:
        def c() -> Iterator[bool]:
            def d(j: int) -> bool:
                return "".join((chr(j), chr(i))).isidentifier()

            return map(d, id_start)

        return all(c())

    id_continue_pos: Set[int] = set(range(0x10FFFF)) - id_start
    return chain.from_iterable(
        starmap(
            _b,
            groupby(zip(filter(b, id_continue_pos), count()), lambda t: t[0] - t[1]),
        )
    )


def _b(_: int, group: Iterator[Tuple[int, int]]) -> Tuple[int, int]:
    t = tuple(e[0] for e in group)
    return (min(t), max(t))


def _d(i: int) -> bool:
    return chr(i).isidentifier()


ranges = _ranges(724, _a(set(filter(_d, range(0x10FFFF)))))

utf8_id_continue.write_text(
    subn(
        r"\bUtf8IdContinue\b[^;]+",
        f"Utf8IdContinue = {ranges}",
        utf8_id_continue.read_text(),
        1,
        MULTILINE,
    )[0]
)
