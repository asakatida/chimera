from functools import reduce
from itertools import repeat
from pathlib import Path
from re import MULTILINE, Pattern, compile
from sys import argv

SOURCE = Path(__file__).parent.parent.resolve()
IGNORE = (
    # patches need to stay as is
    (SOURCE / "patches"),
    # fuzz corpus is ignored for utf8 nonsense
    (SOURCE / "unit_tests" / "fuzz" / "corpus"),
    (SOURCE / "unit_tests" / "fuzz" / "crashes"),
)
SEARCHES = (
    compile(
        rb"$\s^$(?P<keep>\s{2,})(?=\S)(?!(?:private|protected):)", MULTILINE
    ),  # no runs of blank lines
    compile(rb"((?=[^\n])\s)+(?P<keep>$)", MULTILINE),  # no line ending space
    compile(rb"(?P<keep>\s)\s+\Z"),  # no trailing space
)


def test(f: Path) -> bool:
    return (
        (not any(map(f.resolve().is_relative_to, IGNORE)))
        # markdown is ignored
        # python linting is covered by black
        and f.suffix not in (".md", ".py")
        and any(map(Pattern.search, SEARCHES, repeat(f.read_bytes())))
    )


files = list(filter(test, filter(Path.is_file, map(Path, argv[1:]))))
while files:
    for file in files:
        file.write_bytes(
            reduce(
                lambda content, search: search.sub(rb"\g<keep>", content),
                SEARCHES,
                file.read_bytes(),
            )
        )
    files = list(filter(test, files))
