from asyncio import run
from asyncio.subprocess import PIPE
from os import environ
from re import compile
from sys import stderr
from typing import Awaitable, Sequence

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, cmd
from g_ls_tree import g_ls_tree

IN_CI = environ.get("CI", "") == "true"


def ci_args(*args: object) -> tuple[object, ...]:
    return args if IN_CI else ()


async def lint(*args: object) -> bytes:
    return await cmd(*args, err=PIPE, log=False, out=PIPE, timeout=300)


async def black(files: Sequence[object]) -> bytes:
    return await lint("black", *ci_args("--check", "--diff"), "--preview", *files)


async def isort(files: Sequence[object]) -> bytes:
    return await lint("isort", *ci_args("--check-only"), *files)


async def pylama(files: Sequence[object]) -> bytes:
    return await lint("pylama", *files)


async def mypy(files: Sequence[object]) -> bytes:
    return await lint("mypy", *files)


async def main() -> None:
    files = await g_ls_tree("py")
    files_mypy = await g_ls_tree("py", exclude=compile(r"stdlib/|.*/stdlib/"))
    jobs: list[Awaitable[bytes]] = []
    if files:
        jobs.extend((black(files), isort(files), pylama(files)))
    if files_mypy:
        jobs.append(mypy(files_mypy))
    if jobs:
        async for result in as_completed(jobs):
            if result := result.strip():
                print(result.decode())


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
