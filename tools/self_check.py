from asyncio import run
from asyncio.subprocess import PIPE
from re import compile
from sys import stderr
from typing import Sequence

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, ci_args, cmd
from g_ls_tree import g_ls_tree


async def lint(*args: object) -> bytes:
    return await cmd(*args, err=PIPE, log=False, out=PIPE, timeout=300)


async def black(files: Sequence[object]) -> bytes:
    if files:
        return await lint("black", *ci_args("--check", "--diff"), "--preview", *files)
    return b""


async def isort(files: Sequence[object]) -> bytes:
    if files:
        return await lint("isort", *ci_args("--check-only"), *files)
    return b""


async def pylama(files: Sequence[object]) -> bytes:
    if files:
        return await lint("pylama", *files)
    return b""


async def mypy(files: Sequence[object]) -> bytes:
    if files:
        return await lint("mypy", *files)
    return b""


async def main() -> None:
    files = await g_ls_tree("py")
    files_mypy = await g_ls_tree("py", exclude=compile(r"stdlib/|.*/stdlib/"))
    set(
        map(
            print,
            map(
                bytes.decode,
                filter(
                    None,
                    map(
                        bytes.strip,
                        await as_completed(
                            iter(
                                (
                                    black(files),
                                    isort(files),
                                    pylama(files),
                                    mypy(files_mypy),
                                )
                            )
                        ),
                    ),
                ),
            ),
        )
    )


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
