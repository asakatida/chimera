# This script is a wrapper around git ls-tree that adds the ability to
# filter by file extension. It is used by the build system to generate
# lists of files to be processed by other tools.

from asyncio import run
from asyncio.subprocess import PIPE
from itertools import chain, filterfalse
from os import environ
from pathlib import Path
from sys import stderr
from typing import Optional, Pattern

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, chunks, cmd, splitlines

CACHE: dict[str, list[Path]] = {}
IN_CI = environ.get("CI", "") == "true"
SOURCE = Path(__file__).parent.parent.resolve()


async def git_cmd(*args: object) -> bytes:
    return await cmd("git", *args, err=PIPE, log=False, out=PIPE, timeout=60)


async def g_ls_tree(*args: str, exclude: Optional[Pattern[str]] = None) -> list[Path]:
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
                                lambda args: git_cmd(
                                    "ls-tree",
                                    "--full-tree",
                                    "--name-only",
                                    "HEAD",
                                    *args,
                                ),
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
                                    f"{environ.get('BASE_COMMIT', 'HEAD')}^",
                                    "--",
                                    *args,
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


async def main() -> None:
    from argparse import ArgumentParser
    from re import compile

    parser = ArgumentParser()
    parser.add_argument("extensions", nargs="+", help="file extensions to filter by")
    parser.add_argument("-e", "--exclude", help="exclude files matching this regex")
    args = parser.parse_args()

    exclude = compile(args.exclude) if args.exclude else None
    print(*await g_ls_tree(*args.extensions, exclude=exclude), end="", sep="\0")


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
