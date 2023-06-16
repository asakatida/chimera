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

"""generate_utf8_id_insane.py."""

from itertools import product, repeat
from pathlib import Path
from re import MULTILINE, subn
from typing import Iterable

from asyncio_cmd import chunks
from tqdm import tqdm


def _slices(total: int, it: Iterable[int]) -> Iterable[Iterable[str]]:
    return chunks(tqdm(map(hex, it), total=total), 64)


def _ranges(total: int, it: Iterable[int]) -> str:
    ranges = ">, tao::pegtl::utf8::ranges<".join(map(",".join, _slices(total, it)))
    return f"sor<tao::pegtl::utf8::ranges<{ranges}>>"


utf8_id_continue = (
    Path(__file__).parent.parent / "library" / "grammar" / "utf8_id_continue.hpp"
).resolve()

id_start = set(filter(str.isidentifier, map(chr, range(0x10FFFF))))
id_continue = set(
    tqdm(
        filter(
            lambda i: all(  # type: ignore
                map(str.isidentifier, map("".join, repeat(i), map(chr, id_start)))  # type: ignore
            ),
            set(map(chr, range(0x10FFFF))) - id_start,
        ),
        total=2908,
    )
)

ranges = _ranges(
    1234,
    next(  # type: ignore
        tqdm(
            filter(
                str.isidentifier,
                map(
                    "".join,
                    product(
                        id_start,
                        set(map(chr, range(0x10FFFF))).difference(
                            id_start, id_continue
                        ),
                    ),
                ),
            ),
            total=1,
        )
    ).encode(),
)

utf8_id_continue.write_text(
    subn(
        r"\bUtf8IdContinue\b[^;]+",
        f"Utf8IdContinue = {ranges}",
        utf8_id_continue.read_text(),
        1,
        MULTILINE,
    )[0]
)
