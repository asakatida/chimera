from pathlib import Path
from re import MULTILINE, VERBOSE, compile
from sys import argv

SOURCE = Path(__file__).parent.parent.resolve()
IGNORE = (
    # patches need to stay as is
    (SOURCE / "patches"),
    # fuzz corpus is ignored for utf8 nonsense
    (SOURCE / "unit_tests" / "fuzz" / "corpus"),
    (SOURCE / "unit_tests" / "fuzz" / "crashes"),
)
SEARCH = compile(
    rb"""
        $\s^$(?P<keep1>\s{2,})(?=\S)(?!(?:private|protected):)  # no runs of blank lines
        |
        ((?=[^\n])\s)+$  # no line ending space
        |
        (?P<keep2>\s)\s+\Z  # no trailing space
    """,
    MULTILINE | VERBOSE,
)


def test(f: Path) -> bool:
    return (
        (not any(f.resolve().is_relative_to(ignore) for ignore in IGNORE))
        # markdown is ignored
        # python linting is covered by black
        and f.suffix not in (".md", ".py")
        and SEARCH.search(f.read_bytes()) is not None
    )


files = [path for path in (Path(arg) for arg in argv[1:]) if path.is_file()]
while files := [file for file in files if test(file)]:
    for file in files:
        file.write_bytes(SEARCH.sub(rb"\g<keep1>\g<keep2>", file.read_bytes()))
