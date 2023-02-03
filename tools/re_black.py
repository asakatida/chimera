from asyncio import run
from re import sub
from sys import stderr

from asyncio_cmd import ProcessError
from g_ls_tree import g_ls_tree


async def main() -> None:
    sum(
        map(
            lambda path: path.write_text(
                sub(r",\s+([\]})])", r"\g<1>", path.read_text())
            ),
            await g_ls_tree("py"),
        )
    )


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
