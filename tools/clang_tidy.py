from asyncio import run
from itertools import repeat
from sys import argv, stderr

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, cmd_no_timeout
from g_ls_tree import g_ls_tree


async def clang_tidy_fix(build: str) -> None:
    await cmd_no_timeout(
        "clang-tidy",
        f"-p={build}",
        "-quiet",
        "-fix",
        "-fix-errors",
        "-fix-notes",
        *await g_ls_tree("cpp"),
    )


async def clang_tidy(build: str, checks: str) -> None:
    if checks:
        await cmd_no_timeout(
            "clang-tidy", f"-p={build}", f"-checks=-*,{checks}", *await g_ls_tree("cpp")
        )
    else:
        await cmd_no_timeout("clang-tidy", f"-p={build}", *await g_ls_tree("cpp"))


async def main(build: str, *args: str) -> None:
    if not await g_ls_tree("cpp"):
        return
    if args:
        async for _ in as_completed(map(clang_tidy, repeat(build), ("",)), limit=4):
            pass
    else:
        await clang_tidy_fix(build)


if __name__ == "__main__":
    try:
        run(main(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
