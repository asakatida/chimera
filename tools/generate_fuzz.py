from base64 import b64decode
from json import load
from pathlib import Path
from string import printable, whitespace
from sys import argv

PRINTABLE = set(map(ord, set(printable) - set(whitespace) - set('?"\\') | set(" \t")))

PREFIX = """
#include <catch2/catch_test_macros.hpp>

using namespace std::literals;

void TestOne(std::string_view data);

"""


def sanitize(data: bytes) -> str:
    return (
        "".join(map(lambda c: (chr(c) if c in PRINTABLE else f'""\\{hex(c)}""'), data))
        .strip('"')
        .replace('""\\0x', "\\0x")
    )


def make_tests(test_cpp: Path, cases: dict[bytes, bytes], prefix: str) -> None:
    content = PREFIX + "\n".join(
        map(
            lambda test_name, test_data: (
                f'TEST_CASE("{prefix} `{test_name}`") {{'
                f' CHECK_NOTHROW(TestOne("{test_data}")); }}'
            ),
            map(
                lambda name: name.replace("\\", r"\\"),
                map(sanitize, cases.keys()),
            ),
            map(sanitize, cases.values()),
        )
    )
    if test_cpp.exists() and test_cpp.read_text() == content:
        return
    test_cpp.write_text(content)


def main(output_dir: str) -> None:
    with Path(
        "external/big-list-of-naughty-strings/blns.base64.json"
    ).open() as istream:
        blns_base64 = load(istream)
    make_tests(
        Path(output_dir) / "fuzz-base64.cpp",
        dict(zip(map(str.encode, blns_base64), map(str.encode, blns_base64))),
        "fuzz big list of nasty strings base64",
    )
    make_tests(
        Path(output_dir) / "fuzz-blns.cpp",
        dict(zip(map(b64decode, blns_base64), map(b64decode, blns_base64))),
        "fuzz big list of nasty strings",
    )
    with Path("unit_tests/fuzz/cases.json").open() as istream:
        cases = load(istream)
    make_tests(
        Path(output_dir) / "fuzz-cases.cpp",
        dict(zip(map(str.encode, cases.keys()), map(str.encode, cases.values()))),
        "fuzz discovered cases",
    )


if __name__ == "__main__":
    main(*argv[1:])
