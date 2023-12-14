from base64 import b64decode, b64encode
from itertools import cycle, groupby
from json import load
from lzma import decompress
from operator import itemgetter
from pathlib import Path
from string import printable, whitespace
from sys import argv

PRINTABLE = {
    ord(c) for c in {c for c in printable}.difference(whitespace, '?"\\').union(" \t")
}

PREFIX = """
#include <catch2/catch_test_macros.hpp>
#include "virtual_machine/fuzz.hpp"
"""


def sanitize(data: bytes) -> str:
    return (
        "".join((chr(c) if c in PRINTABLE else f'""\\{hex(c)}""') for c in data)
        .strip('"')
        .replace('""\\0x', "\\0x")
    )


def make_tests(*test_cpps: Path, tests: dict[str, dict[bytes, str]]) -> None:
    for test_cpp, test_cases in groupby(
        sorted(
            zip(
                cycle(test_cpps),
                (
                    f'TEST_CASE("fuzz {prefix} `{test_name}`") {{'
                    f"CHECK_NOTHROW(TestOne({test_data}));}}"
                    for prefix, cases in tests.items()
                    for test_name, test_data in zip(
                        (
                            name.replace("\\", r"\\")
                            for name in (sanitize(case) for case in cases.keys())
                        ),
                        cases.values(),
                    )
                ),
            )
        ),
        itemgetter(0),
    ):
        content = PREFIX + "\n".join(test_case[1] for test_case in test_cases)
        if test_cpp.exists() and test_cpp.read_text() == content:
            continue
        test_cpp.parent.mkdir(exist_ok=True, parents=True)
        test_cpp.write_text(content)


def generate_fuzz(*outputs: str) -> None:
    with Path(
        "external/big-list-of-naughty-strings/blns.base64.json"
    ).open() as istream:
        blns_base64 = load(istream)
    with Path("unit_tests/fuzz/cases.json").open() as istream:
        cases = load(istream)
    make_tests(
        *(Path(out) for out in outputs),
        tests={
            "big list of nasty strings base64": {
                key: value
                for key, value in zip(
                    (blns.encode() for blns in blns_base64),
                    (f'"{case}"' for case in blns_base64),
                )
            },
            "big list of nasty strings": {
                key: value
                for key, value in zip(
                    (blns.encode() for blns in blns_base64),
                    (f'base64_decode("{case}")' for case in blns_base64),
                )
            },
            "discovered cases": {
                key: value
                for key, value in zip(
                    (case.encode() for case in cases.keys()),
                    (
                        f'base64_decode("{b64encode(decompress(b64decode(case))).decode()}")'
                        for case in cases.values()
                    ),
                )
            },
        },
    )


if __name__ == "__main__":
    generate_fuzz(*argv[1:])
