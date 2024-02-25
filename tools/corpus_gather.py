"""corpus_gather.py"""

from asyncio import run
from hashlib import sha256
from os import environ
from pathlib import Path
from re import compile
from subprocess import PIPE
from sys import argv
from typing import Iterable

from asyncio_as_completed import as_completed
from asyncio_cmd import cmd, main, splitlines
from corpus_utils import c_tqdm, corpus_objects, corpus_trim, gather_paths


async def corpus_deletions(
    *paths: str, base_reference: str, disable_bars: bool | None
) -> Iterable[str]:
    return (
        line
        for match in c_tqdm(
            compile(rb"commit:.+:(?P<sha>.+)(?P<paths>(?:\s+(?!commit:).+)+)").finditer(
                await cmd(
                    "git",
                    "log",
                    *("--all",) if base_reference.startswith("^") else (),
                    "--date=iso",
                    "--diff-filter=D",
                    "--name-only",
                    "--pretty=format:commit:%cd:%h",
                    base_reference,
                    "--",
                    *paths,
                    out=PIPE,
                )
            ),
            "Commits",
            disable_bars,
        )
        for line in splitlines(match["paths"])
        if any(line.startswith(path) for path in paths) and Path(line).exists()
    )


async def corpus_gather(
    *paths: str,
    base_reference: str = environ.get("BASE_REF", "HEAD"),
    disable_bars: bool | None,
) -> None:
    exclude = {
        line.strip().decode()
        for line in await as_completed(
            c_tqdm(
                (
                    cmd("git", "hash-object", case, out=PIPE, log=False)
                    for case in gather_paths()
                ),
                "Gather existing corpus objects",
                disable_bars,
            )
        )
    }
    for path in paths:
        Path(path).mkdir(exist_ok=True, parents=True)
        for _, case in await corpus_objects(
            path,
            base_reference=base_reference,
            disable_bars=disable_bars,
            exclude=exclude,
        ):
            new_file = Path(path) / sha256(case).hexdigest()
            new_file.write_bytes(case)
    for path in await corpus_deletions(
        *paths, base_reference=base_reference, disable_bars=disable_bars
    ):
        Path(path).unlink()
    corpus_trim(disable_bars=disable_bars)


async def corpus_gather_main(ref: str) -> None:
    if ref == "refs/heads/stable":
        await corpus_gather(
            "unit_tests/fuzz/crashes", "unit_tests/fuzz/corpus", disable_bars=False
        )


if __name__ == "__main__":
    with main():
        run(corpus_gather_main(*argv[1:]))
