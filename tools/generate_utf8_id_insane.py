"""generate_utf8_id_insane.py."""

from pathlib import Path
from re import sub
from typing import Iterable

from asyncio_cmd import chunks


def _ranges(it: Iterable[int]) -> str:
    ranges = ">,tao::pegtl::utf8::ranges<".join(
        ",".join(slc) for slc in chunks((hex(i) for i in it), 64)
    )
    return f"sor<tao::pegtl::utf8::ranges<{ranges}>>"


utf8_id_continue = (
    Path(__file__).parent.parent / "library" / "grammar" / "utf8_id_continue.hpp"
).resolve()

id_start = {i for i in range(0x110000) if chr(i).isidentifier()}

ranges = _ranges(
    next(
        s
        for s in ("".join((chr(s), chr(i))) for s in id_start for i in range(0x110000))
        if s.isidentifier()
    ).encode()
)

utf8_id_continue.write_text(
    sub(
        r"\bUtf8IdContinue\b[^;]+",
        f"Utf8IdContinue = {ranges}",
        utf8_id_continue.read_text(),
    )
)
