from asyncio import run
from re import sub

from asyncio_cmd import main
from g_ls_tree import g_ls_tree


async def re_black() -> None:
    set(
        map(
            lambda path: path.write_text(sub(r",\s+(?=[\]})])", "", path.read_text())),
            await g_ls_tree("py"),
        )
    )


if __name__ == "__main__":
    with main():
        run(re_black())
