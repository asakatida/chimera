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

from itertools import chain, count, groupby
from pathlib import Path
from re import MULTILINE, subn
from typing import Iterable, Set

from asyncio_cmd import chunks
from tqdm import tqdm

utf8_id_continue = (
    Path(__file__).parent.parent / "library" / "grammar" / "utf8_id_continue.hpp"
).resolve()


def _slices(total: int, it: Iterable[int]) -> Iterable[Iterable[str]]:
    return chunks(tqdm(map(hex, it), total=total), 64)


def _ranges(total: int, it: Iterable[int]) -> str:
    ranges = ">, ranges<".join(map(",".join, _slices(total, it)))
    return f"sor<ranges<{ranges}>>"


def _a(id_start: Set[int], end: int) -> Iterable[int]:
    def b(i: int) -> bool:
        def c() -> Iterable[bool]:
            def d(j: int) -> bool:
                return "".join((chr(j), chr(i))).isidentifier()

            return map(d, id_start)

        return all(c())

    id_continue_pos: Set[int] = set(range(end)) - id_start
    return chain.from_iterable(
        map(
            _b, groupby(zip(filter(b, id_continue_pos), count()), lambda t: t[0] - t[1])
        )
    )


def _b(t: tuple[int, Iterable[tuple[int, int]]]) -> tuple[int, int]:
    groups = tuple(e[0] for e in t[1])
    return (min(groups), max(groups))


def _d(i: int) -> bool:
    return chr(i).isidentifier()


ranges = iter(
    (
        _ranges(2, _a(set(filter(_d, range(127))), 127)),
        _ranges(748, _a(set(filter(_d, range(0x10FFFF))), 0x10FFFF)),
    )
)

utf8_id_continue.write_text(
    subn(
        r"\bUtf8IdContinue\b[^;]+",
        lambda _: f"Utf8IdContinue = {next(ranges)}",
        utf8_id_continue.read_text(),
        1,
        MULTILINE,
    )[0]
)
