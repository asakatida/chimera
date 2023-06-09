from asyncio import run
from sys import argv

from asyncio_as_completed import as_completed
from asyncio_cmd import chunks, ci_args, cmd, main
from g_ls_tree import g_ls_tree


async def clang_tidy(build: str) -> None:
    await as_completed(
        map(
            lambda files: cmd(
                "clang-tidy",
                f"-p={build}",
                *ci_args("-fix-errors", "-fix-notes", "-fix", invert=True),
                "-quiet",
                *files,
                out=None,
                timeout=None,
            ),
            chunks(await g_ls_tree("cpp"), 6),
        ),
        cancel=True,
        limit=4,
    )


if __name__ == "__main__":
    with main():
        run(clang_tidy(*argv[1:]))
