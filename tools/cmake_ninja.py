"""cmake_ninja.py"""

from asyncio import run
from os import chdir, environ
from pathlib import Path
from sys import argv

from asyncio_cmd import cmd, main
from chimera_path import cppflags
from ninja import ninja


async def apply_patches() -> None:
    for patch in Path("patches").glob("*"):
        external = Path("external") / patch.name
        await cmd("git", "-C", external, "restore", ".")
        await cmd("git", "-C", external, "apply", patch.resolve())


async def cmake_ninja(build: str = "build", *args: object) -> None:
    chdir(Path(__file__).parent.parent)
    environ["CPPFLAGS"] = " ".join((cppflags, environ.get("CPPFLAGS", "")))
    environ["CXXFLAGS"] = " ".join((cppflags, environ.get("CXXFLAGS", "")))
    await apply_patches()
    await cmd("cmake", "-G", "Ninja", "-B", build, "-S", ".")
    await ninja(build, "unit-test")
    await ninja(build, "test")
    if args:
        await ninja(build, *args)


if __name__ == "__main__":
    with main():
        run(cmake_ninja(*argv[1:]))
