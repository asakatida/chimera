from functools import reduce
from pathlib import Path
from re import MULTILINE, compile
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
        (not any(f.resolve().is_relative_to(ignore) for ignore in IGNORE))
        # markdown is ignored
        # python linting is covered by black
        and f.suffix not in (".md", ".py")
        and any(search.search(f.read_bytes()) for search in SEARCHES)
    )


files = [
    path for path in (Path(arg) for arg in argv[1:]) if path.is_file() and test(path)
]
while files:
    for file in files:
        file.write_bytes(
            reduce(
                lambda content, search: search.sub(rb"\g<keep>", content),
                SEARCHES,
                file.read_bytes(),
            )
        )
    files = [file for file in files if test(file)]
