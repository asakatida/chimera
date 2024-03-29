"""cmake_codecov.py"""

from asyncio import run
from os import chdir, environ
from pathlib import Path
from sys import argv

from asyncio_cmd import cmd, main
from chimera_path import cppflags
from cmake_ninja import apply_patches
from ninja import ninja


async def cmake_codecov(*args: object) -> None:
    chdir(Path(__file__).parent.parent)
    environ["CPPFLAGS"] = " ".join((cppflags, environ.get("CPPFLAGS", "")))
    environ["CXXFLAGS"] = " ".join((
        "-Wall",
        "-Wpedantic",
        "-Werror",
        cppflags,
        "-fcoverage-mapping",
        "-fprofile-instr-generate",
        "-mllvm",
        "-runtime-counter-relocation",
        environ.get("CXXFLAGS", ""),
    ))
    environ["LDFLAGS"] = " ".join(
        ("-Wno-unused-command-line-argument", environ.get("LDFLAGS", ""))
    )
    await apply_patches()
    await cmd("cmake", "-G", "Ninja", "-B", "build", "-S", ".")
    await ninja("build", *args)


if __name__ == "__main__":
    with main():
        run(cmake_codecov(*argv[1:]))
