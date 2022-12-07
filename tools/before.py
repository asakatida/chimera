from asyncio import run
from asyncio.subprocess import PIPE
from os import environ
from sys import stderr

from asyncio_cmd import ProcessError, cmd_env


async def main() -> None:
    for line in (
        await cmd_env(
            {"PATH": environ["PATH"], "PWD": environ["PWD"]},
            "pip",
            "install",
            "-r",
            "requirements.txt",
            stdout=PIPE,
            timeout=120,
        )
    ).splitlines():
        if not line.startswith(b"Requirement already satisfied: "):
            print(line.decode(), file=stderr)


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
