from asyncio import gather, run
from itertools import repeat
from pathlib import Path
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd
from g_ls_tree import g_ls_tree


async def clang_tidy_fix(build: str) -> None:
    with (Path(build) / "clang-tidy.log").open("w") as log:
        await cmd(
            "clang-tidy",
            f"-p={build}",
            "-quiet",
            "-fix",
            "-fix-errors",
            "-fix-notes",
            *await g_ls_tree("cpp"),
            stdout=log,
            timeout=1200,
        )


async def clang_tidy(build: str, checks: str) -> None:
    if checks:
        await cmd(
            "clang-tidy",
            f"-p={build}",
            f"-checks=-*,{checks}",
            *await g_ls_tree("cpp"),
            timeout=1200,
        )
    else:
        await cmd(
            "clang-tidy",
            f"-p={build}",
            *await g_ls_tree("cpp"),
            timeout=1200,
        )


async def main(build: str, *args: str) -> None:
    if args:
        await gather(*map(clang_tidy, repeat(build), ("", *args)))
    else:
        await clang_tidy_fix(build)


if __name__ == "__main__":
    try:
        run(main(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
