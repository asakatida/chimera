"""crash_reset.py"""

from pathlib import Path
from sys import argv

from asyncio_cmd import main

SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"


def crash_reset() -> None:
    CRASHES.mkdir(exist_ok=True, parents=True)
    for crash in (path for path in CRASHES.rglob("*") if path.is_file()):
        crash.rename(CORPUS / crash.relative_to(CRASHES))


if __name__ == "__main__":
    with main():
        crash_reset(*argv[1:])
