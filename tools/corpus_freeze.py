"""corpus_freeze.py"""

from asyncio import run
from base64 import b64encode
from json import dump, load
from lzma import compress
from os import environ
from pathlib import Path
from subprocess import PIPE
from sys import argv

from asyncio_as_completed import as_completed
from asyncio_cmd import cmd, main
from corpus_ascii import is_ascii
from corpus_utils import c_tqdm, corpus_objects

BAD_KEYS = {
    "1e445d9da094d2116c048d181b9d6247f497da6e",
    "4c8b5364cfa29a8a932c60061863c327aea88cce",
    "af7fa358bc47be3966feee502f2c62a39856711f",
}


async def corpus_freeze(
    output: str,
    *,
    base_reference: str = environ.get("BASE_REF", "^origin/stable"),
    disable_bars: bool | None,
) -> None:
    file = Path(output)
    with file.open() as istream:
        cases = {key: value for key, value in load(istream).items()}
    crashes = [
        path for path in Path("unit_tests/fuzz/crashes").rglob("*") if path.is_file()
    ]
    existing = {
        key.strip().decode()
        for key in await as_completed(
            c_tqdm(
                (
                    cmd("git", "hash-object", path, log=False, out=PIPE)
                    for path in crashes
                ),
                "Hash corpus",
                disable_bars,
                total=len(crashes),
            )
        )
    }
    for key in BAD_KEYS.union(existing).intersection(cases.keys()):
        del cases[key]
    cases.update(
        (sha, b64encode(compress(obj)).decode())
        for sha, obj in await corpus_objects(
            "unit_tests/fuzz/corpus",
            "unit_tests/fuzz/crashes",
            base_reference=base_reference,
            disable_bars=disable_bars,
            exclude=BAD_KEYS.union(existing).union(cases.keys()),
        )
        if is_ascii(obj)
    )
    with file.open("w") as ostream:
        dump(cases, ostream, indent=4, sort_keys=True)


async def corpus_freeze_main(output: str) -> None:
    await corpus_freeze(output, disable_bars=None)


if __name__ == "__main__":
    with main():
        run(corpus_freeze_main(*argv[1:]))
