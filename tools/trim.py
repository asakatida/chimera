from functools import reduce
from pathlib import Path
from re import MULTILINE, Match, compile
from sys import argv

SEARCHES = (
    compile(
        r"$\s^$(?P<keep>\s{2,})(?=\S)(?!private:)", MULTILINE
    ),  # no runs of blank lines
    compile(r"((?=[^\n])\s)+(?P<keep>$)", MULTILINE),  # no line ending space
    compile(r"^(?P<keep>( +\t *| *\t +))", MULTILINE),  # no mixing tabs with spaces
)


def sub(match: Match[str]) -> str:
    keep = match.group("keep")
    if "\t" in keep and "\n" not in keep:
        return compile(r" {2}").sub("\t", keep)
    return keep


def test(f: Path) -> bool:
    # markdown is ignored
    # python linting is covered elsewhere
    return f.suffix not in (".md", ".py") and any(
        map(lambda search: search.search(f.read_text()), SEARCHES)
    )


files = list(filter(test, filter(Path.is_file, map(Path, argv[1:]))))
while files:
    for file in files:
        file.write_text(
            reduce(
                lambda content, search: search.sub(sub, content),
                SEARCHES,
                file.read_text(),
            )
        )
    files = list(filter(test, files))
