from asyncio import gather, run
from itertools import repeat
from pathlib import Path
from sys import argv, stderr
from typing import Sequence

from asyncio_cmd import ProcessError, cmd
from g_ls_tree import g_ls_tree


async def clang_tidy_fix(build: str, files: Sequence[object]) -> None:
    with (Path(build) / "clang-tidy.log").open("w") as log:
        await cmd(
            "clang-tidy",
            f"-p={build}",
            "-quiet",
            "-fix",
            "-fix-errors",
            "-fix-notes",
            *files,
            stdout=log,
            timeout=3600,
        )


async def clang_tidy(build: str, checks: str, files: Sequence[object]) -> None:
    if checks:
        await cmd(
            "clang-tidy",
            f"-p={build}",
            f"-checks=-*,{checks}",
            *files,
            stdout=None,
            timeout=3600,
        )
    else:
        await cmd(
            "clang-tidy",
            f"-p={build}",
            *files,
            stdout=None,
            timeout=3600,
        )


async def main(build: str, *args: str) -> None:
    files = await g_ls_tree("cpp")
    if not files:
        return
    if args:
        await gather(*map(clang_tidy, repeat(build), ("", *args), repeat(files)))
    else:
        await clang_tidy_fix(build, files)


if __name__ == "__main__":
    try:
        run(main(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
