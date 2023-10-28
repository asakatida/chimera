from base64 import b64decode, b64encode
from itertools import chain, cycle, groupby
from json import load
from lzma import decompress
from operator import itemgetter
from pathlib import Path
from string import printable, whitespace
from sys import argv

PRINTABLE = set(map(ord, set(printable) - set(whitespace) - set('?"\\') | set(" \t")))

PREFIX = """
#include <catch2/catch_test_macros.hpp>
#include "virtual_machine/fuzz.hpp"
"""


def extract_case(case: bytes) -> bytes:
    try:
        return decompress(case)
    except Exception:
        return case


def sanitize(data: bytes) -> str:
    return (
        "".join(map(lambda c: (chr(c) if c in PRINTABLE else f'""\\{hex(c)}""'), data))
        .strip('"')
        .replace('""\\0x', "\\0x")
    )


def make_tests(*test_cpps: Path, tests: dict[str, dict[bytes, str]]) -> None:
    for test_cpp, test_cases in groupby(
        sorted(
            zip(
                cycle(test_cpps),
                chain.from_iterable(
                    map(
                        lambda prefix, cases: map(
                            lambda test_name, test_data: (
                                f'TEST_CASE("fuzz {prefix} `{test_name}`") {{'
                                f"CHECK_NOTHROW(TestOne({test_data}));}}"
                            ),
                            map(
                                lambda name: name.replace("\\", r"\\"),
                                map(sanitize, cases.keys()),
                            ),
                            cases.values(),
                        ),
                        tests.keys(),
                        tests.values(),
                    )
                ),
            )
        ),
        itemgetter(0),
    ):
        content = PREFIX + "\n".join(map(itemgetter(1), test_cases))
        if test_cpp.exists() and test_cpp.read_text() == content:
            continue
        test_cpp.parent.mkdir(parents=True, exist_ok=True)
        test_cpp.write_text(content)


def main(*outputs: str) -> None:
    with Path(
        "external/big-list-of-naughty-strings/blns.base64.json"
    ).open() as istream:
        blns_base64 = load(istream)
    with Path("unit_tests/fuzz/cases.json").open() as istream:
        cases = load(istream)
    make_tests(
        *map(Path, outputs),
        tests={
            "big list of nasty strings base64": dict(
                zip(map(str.encode, blns_base64), blns_base64)
            ),
            "big list of nasty strings": dict(
                zip(
                    map(str.encode, blns_base64),
                    map(lambda case: f'base64_decode("{case}")', blns_base64),
                )
            ),
            "discovered cases": dict(
                zip(
                    map(str.encode, cases.keys()),
                    map(
                        lambda case: f'base64_decode("{case}")',
                        map(
                            bytes.decode,
                            map(
                                b64encode,
                                map(extract_case, map(b64decode, cases.values())),
                            ),
                        ),
                    ),
                )
            ),
        },
    )


if __name__ == "__main__":
    main(*argv[1:])
