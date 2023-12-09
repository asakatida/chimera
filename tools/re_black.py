from asyncio import run
from re import sub

from asyncio_cmd import main
from g_ls_tree import g_ls_tree


async def re_black() -> None:
    {
        path.write_text(
            sub(r"\s+(?=[\]})])", "", sub(r",\s+(?=[\]})])", "", path.read_text()))
        )
        for path in await g_ls_tree("py")
    }


if __name__ == "__main__":
    with main():
        run(re_black())
