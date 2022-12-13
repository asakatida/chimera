# This script is a wrapper around git ls-tree that adds the ability to
# filter by file extension. It is used by the build system to generate
# lists of files to be processed by other tools.

from asyncio import run
from asyncio.subprocess import PIPE
from itertools import chain, filterfalse
from os import environ
from pathlib import Path
from sys import stderr
from typing import Iterable, Optional, Pattern, Sequence, Union

from asyncio_as_completed import as_completed, raise_errors
from asyncio_cmd import ProcessError, chunks, cmd

CACHE: dict[str, list[Path]] = {}
IN_CI = environ.get("CI", "") == "true"
SOURCE = Path(__file__).resolve().parent.parent


def splitlines(results: Sequence[Union[bytes, BaseException]]) -> Iterable[str]:
    raise_errors("Warning: got multiple errors:", results)
    byte_results: Iterable[bytes] = filter(
        lambda elem: isinstance(elem, bytes), results  # type: ignore
    )
    return chain.from_iterable(
        map(
            str.splitlines,
            map(
                bytes.decode,
                byte_results,
            ),
        )
    )


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
    paths = splitlines(
        list(
            await as_completed(
                map(
                    lambda args: cmd(
                        "git",
                        "ls-tree",
                        "--full-tree",
                        "--name-only",
                        "HEAD",
                        *args,
                        log=False,
                        stdout=PIPE,
                        timeout=60,
                    ),
                    chunks(rglob, 255),
                )
            )
        )
    )
    if IN_CI:
        CACHE[cache_key] = list(map(Path, paths))
        return CACHE[cache_key]
    paths = splitlines(
        list(
            await as_completed(
                map(
                    lambda args: cmd(
                        "git",
                        "diff",
                        "--name-only",
                        "HEAD^",
                        "--",
                        *args,
                        log=False,
                        stdout=PIPE,
                        timeout=60,
                    ),
                    chunks(paths, 255),
                ),
            )
        )
    )
    CACHE[cache_key] = list(map(Path, paths))
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
