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


def _a(id_start: frozenset[int], end: int) -> Iterable[int]:
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
    groups = frozenset(e for e, _ in t)
    return (min(groups), max(groups))


def _d(i: int) -> bool:
    return chr(i).isidentifier()


ranges = iter((
    _ranges(_a(frozenset(i for i in range(0x100) if _d(i)), 0x100)),
    _ranges(_a(frozenset(i for i in range(0x110000) if _d(i)), 0x110000)),
))

utf8_id_continue.write_text(
    sub(
        r"\bUtf8IdContinue\b[^;]+",
        lambda _: f"Utf8IdContinue = {next(ranges)}",
        utf8_id_continue.read_text(),
    )
)
