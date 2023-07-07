# This script is a wrapper around git ls-tree that adds the ability to
# filter by file extension. It is used by the build system to generate
# lists of files to be processed by other tools.

from asyncio import run
from asyncio.subprocess import PIPE
from itertools import chain, filterfalse
from os import environ
from pathlib import Path
from typing import Pattern

from asyncio_as_completed import as_completed
from asyncio_cmd import chunks, cmd, main, splitlines

CACHE: dict[str, list[Path]] = {}
IN_CI = environ.get("CI", "") == "true"
SOURCE = Path(__file__).parent.parent.resolve()


async def git_cmd(*args: object, out: int | None = None) -> bytes:
    return await cmd("git", *args, out=out, log=False)


async def g_ls_tree(*args: str, exclude: Pattern[str] | None = None) -> list[Path]:
    cache_key = f"{args}, {exclude}"
    if cache_key in CACHE:
        return CACHE[cache_key]
    if exclude:
        CACHE[cache_key] = list(
            map(Path, filterfalse(exclude.match, map(str, await g_ls_tree(*args))))
        )
        return CACHE[cache_key]
    rglob = chain.from_iterable(map(SOURCE.rglob, map("*.{}".format, args)))
    CACHE[cache_key] = sorted(
        map(
            Path,
            set(
                chain.from_iterable(
                    map(
                        splitlines,
                        await as_completed(
                            map(
                                lambda args: git_cmd("ls-files", "--", *args, out=PIPE),
                                chunks(rglob, 4096),
                            ),
                            cancel=True,
                        ),
                    )
                )
            ),
        )
    )
    if IN_CI:
        return CACHE[cache_key]
    CACHE[cache_key] = sorted(
        map(
            Path,
            set(
                chain.from_iterable(
                    map(
                        splitlines,
                        await as_completed(
                            map(
                                lambda args: git_cmd(
                                    "diff",
                                    "--name-only",
                                    environ.get("BASE_COMMIT", "HEAD^"),
                                    "--",
                                    *args,
                                    out=PIPE,
                                ),
                                chunks(CACHE[cache_key], 4096),
                            ),
                            cancel=True,
                        ),
                    )
                )
            ),
        )
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
