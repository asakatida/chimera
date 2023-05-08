from asyncio import run
from sys import argv

from asyncio_as_completed import as_completed
from asyncio_cmd import cmd, main
from g_ls_tree import g_ls_tree


async def clang_format() -> None:
    await as_completed(
        map(
            lambda file: cmd("clang-format", "-style=file", "-i", file),
            await g_ls_tree("cpp", "h", "hpp"),
        )
    )


if __name__ == "__main__":
    with main():
        run(clang_format(*argv[1:]))
