from itertools import chain, count, cycle, groupby
from json import load
from operator import itemgetter
from pathlib import Path
from string import printable, whitespace
from sys import argv

PRINTABLE = set(map(ord, set(printable) - set(whitespace) - set('?"\\') | set(" \t")))

PREFIX = """
#include <catch2/catch_test_macros.hpp>
#include "virtual_machine/fuzz.hpp"
"""


def sanitize(data: bytes) -> str:
    return (
        "".join(map(lambda c: (chr(c) if c in PRINTABLE else f'""\\{hex(c)}""'), data))
        .strip('"')
        .replace('""\\0x', "\\0x")
    )


def make_tests(test_cpps: list[Path], tests: dict[str, dict[bytes, str]]) -> None:
    for test_cpp, test_cases in groupby(
        sorted(
            zip(
                cycle(test_cpps),
                chain.from_iterable(
                    map(
                        lambda prefix, cases: map(
                            lambda test_name, test_data: (
                                f'TEST_CASE("fuzz {prefix} `{test_name}`") {{'
                                f" CHECK_NOTHROW(TestOne({test_data})); }}"
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
    blns_base64_file = Path(
        "external/big-list-of-naughty-strings/blns.base64.json"
    ).resolve()
    cases_file = Path("unit_tests/fuzz/cases.json").resolve()
    with blns_base64_file.open() as istream:
        blns_base64 = load(istream)
    with cases_file.open() as istream:
        cases = load(istream)
    make_tests(
        list(map(Path, outputs)),
        {
            "big list of nasty strings base64": dict(
                zip(
                    map(str.encode, blns_base64),
                    map(
                        lambda idx: f'json_from_file("{blns_base64_file}")[{idx}].as<std::string>()',
                        count(),
                    ),
                )
            ),
            "big list of nasty strings": dict(
                zip(
                    map(str.encode, blns_base64),
                    map(
                        lambda idx: f'base64_decode(json_from_file("{blns_base64_file}")[{idx}].as<std::string>())',
                        count(),
                    ),
                )
            ),
            "discovered cases": dict(
                zip(
                    map(str.encode, cases.keys()),
                    map(
                        lambda key: f'base64_decode(json_from_file("{cases_file}")["{key}"].as<std::string>())',
                        cases.keys(),
                    ),
                )
            ),
        },
    )


if __name__ == "__main__":
    main(*argv[1:])
