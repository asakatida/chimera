from asyncio import run
from sys import argv, stderr

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, chunks, ci_args, cmd_no_timeout
from g_ls_tree import g_ls_tree


async def clang_tidy(build: str) -> None:
    await as_completed(
        map(
            lambda files: cmd_no_timeout(
                "clang-tidy",
                f"-p={build}",
                *ci_args("-fix-errors", "-fix-notes", "-fix", invert=True),
                "-quiet",
                *files,
            ),
            chunks(await g_ls_tree("cpp"), 6),
        ),
        cancel=True,
        limit=4,
    )


if __name__ == "__main__":
    try:
        run(clang_tidy(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
