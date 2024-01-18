"""ninja.py"""

from asyncio import run
from sys import argv

from asyncio_cmd import ProcessError, cmd, main


async def ninja_cmd(build: str, *args: object) -> None:
    try:
        await cmd("ninja", "-C", build, "-k0", *args, err=None)
    except ProcessError:
        pass


async def ninja(build: str, *args: object) -> None:
    await ninja_cmd(build, "-j1", "chimera-grammar")
    await ninja_cmd(build, "chimera-core")
    await ninja_cmd(build, "-j1", "fuzzers")
    await cmd("ninja", "-C", build, *args, err=None)


if __name__ == "__main__":
    with main():
        run(ninja(*argv[1:]))
