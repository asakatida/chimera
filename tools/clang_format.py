from asyncio import run
from sys import argv, stderr

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, cmd
from g_ls_tree import g_ls_tree


async def clang_format() -> None:
    await as_completed(
        map(
            lambda file: cmd("clang-format", "-style=file", "-i", file),
            await g_ls_tree("cpp", "h", "hpp"),
        )
    )


if __name__ == "__main__":
    try:
        run(clang_format(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
