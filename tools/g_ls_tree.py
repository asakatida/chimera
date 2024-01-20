# This script is a wrapper around git ls-tree that adds the ability to
# filter by file extension. It is used by the build system to generate
# lists of files to be processed by other tools.

from asyncio import run
from asyncio.subprocess import PIPE
from os import environ
from pathlib import Path
from typing import Pattern

from asyncio_as_completed import as_completed
from asyncio_cmd import chunks, cmd, main, splitlines
from chimera_utils import IN_CI

CACHE: dict[str, list[Path]] = {}
SOURCE = Path(__file__).parent.parent.resolve()


async def git_cmd(*args: object, out: int | None = None) -> bytes:
    return await cmd("git", *args, out=out, log=False)


async def g_ls_tree(
    *args: str,
    base_reference: str = environ.get("BASE_REF", "HEAD^"),
    exclude: Pattern[str] | None = None,
) -> list[Path]:
    cache_key = f"{args}, {exclude}"
    if cache_key in CACHE:
        return CACHE[cache_key]
    if exclude:
        CACHE[cache_key] = [
            Path(path)
            for path in await g_ls_tree(*args)
            if exclude.match(str(path)) is None
        ]
        return CACHE[cache_key]
    rglob = (path for arg in args for path in SOURCE.rglob(f"*.{arg}"))
    CACHE[cache_key] = sorted(
        Path(path)
        for path in {
            line
            for lines in await as_completed(
                git_cmd("ls-files", "--", *args, out=PIPE)
                for args in chunks(rglob, 4096)
            )
            for line in splitlines(lines)
        }
    )
    if IN_CI:
        return CACHE[cache_key]
    CACHE[cache_key] = sorted(
        Path(path)
        for path in {
            line
            for lines in await as_completed(
                git_cmd("diff", "--name-only", base_reference, "--", *args, out=PIPE)
                for args in chunks(CACHE[cache_key], 4096)
            )
            for line in splitlines(lines)
        }
    )
    return CACHE[cache_key]


async def g_ls_tree_main() -> None:
    from argparse import ArgumentParser
    from re import compile

    parser = ArgumentParser()
    parser.add_argument("extensions", nargs="+", help="file extensions to filter by")
    parser.add_argument("-e", "--exclude", help="exclude files matching this regex")
    args = parser.parse_args()

    exclude = compile(args.exclude) if args.exclude else None
    print(*await g_ls_tree(*args.extensions, exclude=exclude), end="", sep="\0")


if __name__ == "__main__":
    with main():
        run(g_ls_tree_main())
