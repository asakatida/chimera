from base64 import b64decode
from hashlib import sha256
from json import load
from pathlib import Path
from string import printable, whitespace
from sys import argv

PRINTABLE = set(map(ord, set(printable) - set(whitespace) - set('?"\\') | set(" \t")))

template = """
#include <catch2/catch_test_macros.hpp>

using namespace std::literals;

void TestOne(std::string_view data);

TEST_CASE("{}") {{ TestOne("{}"); }}
"""


def write_test(test_cpp: Path, content: str) -> None:
    if test_cpp.exists() and test_cpp.read_text() == content:
        return
    test_cpp.write_text(content)


def sanitize(data: bytes) -> str:
    return (
        "".join(map(lambda c: (chr(c) if c in PRINTABLE else f'""\\{hex(c)}""'), data))
        .strip('"')
        .replace('""\\0x', "\\0x")
    )


def make_test(
    test_cpp: Path, test_name: str, test_data: str | bytes, prefix: str
) -> None:
    test_name = sanitize(test_name.encode()).replace("\\", r"\\")
    test_data = (
        sanitize(test_data.encode())
        if isinstance(test_data, str)
        else sanitize(test_data)
    )
    test_name = prefix + " `" + test_name + "`"
    write_test(test_cpp, template.format(test_name, test_data))


def make_tests(test_cpp: Path, cases: dict[str, str], prefix: str) -> None:
    items = zip(
        map(sanitize, map(str.encode, cases.keys())),
        map(sanitize, map(str.encode, cases.values())),
    )
    test_name, test_data = next(items)
    write_test(
        test_cpp,
        template.format(prefix + " `" + test_name + "`", test_data)
        + "\n".join(
            map(
                lambda item: (
                    f'TEST_CASE("{prefix} `{item[0]}`") {{ TestOne("{item[1]}"); }}'
                ),
                items,
            )
        ),
    )


def main(output_dir: str) -> None:
    with Path("external/big-list-of-naughty-strings/blns.json").open() as istream:
        blns = load(istream)
    for test_data in blns:
        make_test(
            Path(output_dir)
            / f"fuzz-blns-{sha256(test_data.encode()).hexdigest()}.cpp",
            test_data,
            test_data.encode(),
            "fuzz big list of nasty strings",
        )
    with Path(
        "external/big-list-of-naughty-strings/blns.base64.json"
    ).open() as istream:
        blns_base64 = load(istream)
    for test_data in blns_base64:
        make_test(
            Path(output_dir)
            / f"fuzz-base64-{sha256(test_data.encode()).hexdigest()}.cpp",
            test_data,
            test_data.encode(),
            "fuzz big list of nasty strings base64",
        )
        # TODO(asakatida): fuzz base64 decode
        test_bytes = b64decode(test_data, validate=True)
        make_test(
            Path(output_dir) / f"fuzz-blns-{sha256(test_bytes).hexdigest()}.cpp",
            sanitize(test_bytes),
            test_bytes,
            "fuzz big list of nasty strings",
        )
    with Path("unit_tests/fuzz/cases.json").open() as istream:
        cases = load(istream)
    make_tests(Path(output_dir) / "fuzz-cases.cpp", cases, "fuzz discovered cases")


if __name__ == "__main__":
    main(*argv[1:])
