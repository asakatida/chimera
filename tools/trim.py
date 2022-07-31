from pathlib import Path
from re import MULTILINE, compile
from sys import argv

SEARCH = compile(r"$\s^$(\s{2,})(?=\S)(?!private:)", MULTILINE)


def test(f: Path) -> bool:
    return f.suffix not in (".md", ".py") and bool(SEARCH.search(f.read_text()))


files = list(filter(test, filter(Path.is_file, map(Path, argv[1:]))))
while files:
    for file in files:
        file.write_text(SEARCH.sub(r"\1", file.read_text()))
    files = list(filter(test, files))
