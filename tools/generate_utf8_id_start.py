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

from itertools import groupby
from pathlib import Path
from re import sub
from typing import Iterable

from asyncio_cmd import chunks

utf8_id_start = (
    Path(__file__).parent.parent / "library" / "grammar" / "utf8_id_start.hpp"
).resolve()


def _ranges(it: Iterable[int]) -> str:
    ranges = ">,tao::pegtl::utf8::ranges<".join(
        ",".join(slc) for slc in chunks((hex(i) for i in it), 64)
    )
    return f"sor<tao::pegtl::utf8::ranges<{ranges}>>"


def _a(_: int, t: Iterable[tuple[int, int]]) -> tuple[int, int]:
    groups = {e for e, _ in t}
    return (min(groups), max(groups))


ranges = iter((
    _ranges(
        i
        for t in groupby(
            zip((i for i in range(0x100) if chr(i).isidentifier()), range(0x100)),
            lambda t: t[0] - t[1],
        )
        for i in _a(*t)
    ),
    _ranges(
        i
        for t in groupby(
            zip((i for i in range(0x110000) if chr(i).isidentifier()), range(0x110000)),
            lambda t: t[0] - t[1],
        )
        for i in _a(*t)
    ),
))

utf8_id_start.write_text(
    sub(
        r"\bUtf8IdStart\b[^;]+",
        lambda _: f"Utf8IdStart = {next(ranges)}",
        utf8_id_start.read_text(),
    )
)
