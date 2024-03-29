"""generate_utf8_space.py."""

from itertools import groupby
from pathlib import Path
from re import sub
from typing import Iterable

from asyncio_cmd import chunks

utf8_space = (
    Path(__file__).parent.parent / "library" / "grammar" / "utf8_space.hpp"
).resolve()


def _ranges(it: Iterable[int]) -> str:
    ranges = ">,tao::pegtl::utf8::ranges<".join(
        ",".join(slc) for slc in chunks((hex(i) for i in it), 64)
    )
    return f"sor<tao::pegtl::utf8::ranges<{ranges}>>"


def _a(_: int, t: Iterable[tuple[int, int]]) -> tuple[int, int]:
    groups = frozenset(e for e, _ in t)
    return (min(groups), max(groups))


ranges = iter((
    _ranges(
        i
        for t in groupby(
            zip((i for i in range(0x100) if chr(i).isspace()), range(0x100)),
            lambda t: t[0] - t[1],
        )
        for i in _a(*t)
    ),
    _ranges(
        i
        for t in groupby(
            zip((i for i in range(0x110000) if chr(i).isspace()), range(0x110000)),
            lambda t: t[0] - t[1],
        )
        for i in _a(*t)
    ),
))

utf8_space.write_text(
    sub(
        r"\bUtf8Space\b[^;]+",
        lambda _: f"Utf8Space = {next(ranges)}",
        utf8_space.read_text(),
    )
)
