from asyncio import run
from itertools import chain
from re import compile
from sys import version_info
from typing import Sequence

from asyncio_as_completed import as_completed
from asyncio_cmd import ci_args, cmd, main, splitlines
from g_ls_tree import g_ls_tree
from structlog import get_logger


async def lint(*args: object) -> bytes:
    return await cmd(*args, log=False, timeout=1200)


async def black(files: Sequence[object]) -> bytes:
    if files:
        return await lint(
            "black",
            *ci_args("--check", "--diff"),
            "--preview",
            "--target-version",
            f"py{version_info.major}{version_info.minor}",
            *files,
        )
    return b""


async def isort(files: Sequence[object]) -> bytes:
    if files:
        return await lint(
            "isort",
            *ci_args("--check-only"),
            "--python-version",
            f"{version_info.major}{version_info.minor}",
            *files,
        )
    return b""


async def pylama(files: Sequence[object]) -> bytes:
    if files:
        return await lint(
            "pylama",
            "--linters",
            # "mccabe,mypy,pycodestyle,pydocstyle,pyflakes,isort",
            "mccabe,pycodestyle,pyflakes,isort",
            *files,
        )
    return b""


async def mypy(files: Sequence[object]) -> bytes:
    if files:
        return await lint(
            "mypy",
            "--python-version",
            f"{version_info.major}.{version_info.minor}",
            *files,
        )
    return b""


async def self_check() -> None:
    files = await g_ls_tree("py")
    files_mypy = await g_ls_tree("py", exclude=compile(r"stdlib/|.*/stdlib/"))
    set(
        map(
            get_logger().info,
            chain.from_iterable(
                map(
                    splitlines,
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
        )
    )


if __name__ == "__main__":
    with main():
        run(self_check())
