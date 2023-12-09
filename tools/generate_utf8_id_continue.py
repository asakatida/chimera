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

from itertools import groupby
from pathlib import Path
from re import MULTILINE, sub
from typing import Iterable

from asyncio_cmd import chunks
from tqdm import tqdm

utf8_id_continue = (
    Path(__file__).parent.parent / "library" / "grammar" / "utf8_id_continue.hpp"
).resolve()


def _slices(total: int, it: Iterable[int]) -> Iterable[Iterable[str]]:
    return chunks(tqdm((hex(i) for i in it), total=total), 64)


def _ranges(total: int, it: Iterable[int]) -> str:
    ranges = ">,tao::pegtl::utf8::ranges<".join(
        ",".join(slc) for slc in _slices(total, it)
    )
    return f"sor<tao::pegtl::utf8::ranges<{ranges}>>"


def _a(id_start: set[int], end: int) -> Iterable[int]:
    def b(i: int) -> bool:
        def c() -> Iterable[bool]:
            def d(j: int) -> bool:
                return "".join((chr(j), chr(i))).isidentifier()

            return (d(j) for j in id_start)

        return all(c())

    id_continue_pos = {i for i in range(end)} - id_start
    return (
        i
        for t in groupby(
            zip((b(i) for i in id_continue_pos), range(len(id_continue_pos))),
            lambda t: t[0] - t[1],
        )
        for i in _b(t)
    )


def _b(t: tuple[int, Iterable[tuple[int, int]]]) -> tuple[int, int]:
    groups = {e[0] for e in t[1]}
    return (min(groups), max(groups))


def _d(i: int) -> bool:
    return chr(i).isidentifier()


ranges = iter(
    (
        _ranges(16, _a({i for i in range(127) if _d(i)}, 0x100)),
        _ranges(724, _a({i for i in range(0x10FFFF) if _d(i)}, 0x10FFFF)),
    )
)

utf8_id_continue.write_text(
    sub(
        r"\bUtf8IdContinue\b[^;]+",
        lambda _: f"Utf8IdContinue = {next(ranges)}",
        utf8_id_continue.read_text(),
        flags=MULTILINE,
    )[0]
)
