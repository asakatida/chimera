from asyncio import run
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd_no_timeout
from g_ls_tree import g_ls_tree


async def clang_tidy(build: str) -> None:
    if not await g_ls_tree("cpp"):
        return
    await cmd_no_timeout(
        "clang-tidy",
        f"-p={build}",
        "-fix-errors",
        "-fix-notes",
        "-fix",
        "-quiet",
        *await g_ls_tree("cpp"),
    )


if __name__ == "__main__":
    try:
        run(clang_tidy(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
