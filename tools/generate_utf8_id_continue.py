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

from itertools import chain, groupby
from pathlib import Path
from re import sub
from typing import Iterable

from asyncio_cmd import chunks

utf8_id_continue = (
    Path(__file__).parent.parent / "library" / "grammar" / "utf8_id_continue.hpp"
).resolve()


def _ranges(it: Iterable[int]) -> str:
    ranges = ">,tao::pegtl::utf8::ranges<".join(
        ",".join(slc) for slc in chunks((hex(i) for i in it), 64)
    )
    return f"sor<tao::pegtl::utf8::ranges<{ranges}>>"


def _a(id_start: set[int], end: int) -> Iterable[int]:
    return (
        i
        for t in groupby(
            zip(
                chain(
                    sorted(id_start),
                    (
                        i
                        for i in range(max(id_start) + 1, end)
                        if all(
                            "".join((chr(j), chr(i))).isidentifier() for j in id_start
                        )
                    ),
                ),
                range(end),
            ),
            lambda t: t[0] - t[1],
        )
        for i in _b(*t)
    )


def _b(_: int, t: Iterable[tuple[int, int]]) -> tuple[int, int]:
    groups = {e for e, _ in t}
    return (min(groups), max(groups))


def _d(i: int) -> bool:
    return chr(i).isidentifier()


ranges = iter((
    _ranges(_a({i for i in range(0x100) if _d(i)}, 0x100)),
    _ranges(_a({i for i in range(0x110000) if _d(i)}, 0x110000)),
))

utf8_id_continue.write_text(
    sub(
        r"\bUtf8IdContinue\b[^;]+",
        lambda _: f"Utf8IdContinue = {next(ranges)}",
        utf8_id_continue.read_text(),
    )
)
