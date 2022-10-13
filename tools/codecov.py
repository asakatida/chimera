# Copyright (c) 2022 Asa Katida <github@holomaplefeline.net>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""codecov.py"""

from asyncio import run
from os import chdir, environ
from pathlib import Path
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd
from ninja import ninja


async def main(output: str) -> None:
    output = str(Path(output).resolve())
    environ["CMAKE_BUILD_TYPE"] = "Coverage"
    environ["CC"] = "/usr/bin/clang"
    environ["CXX"] = "/usr/bin/clang++"
    environ["CXXFLAGS"] = " ".join(
        (
            environ.get("CXXFLAGS", "-O1 -DNDEBUG").strip(),
            "-fcoverage-mapping",
            "-fprofile-instr-generate",
            "-mllvm",
            "-runtime-counter-relocation",
        )
    )
    llvm_profile_file = Path(
        environ.get(
            "LLVM_PROFILE_FILE",
            Path(environ.get("LLVM_PROFILE_DIR", "/tmp/coverage"))
            / "llvm-profile.%c%p.profraw",
        )
    ).resolve()
    environ["LLVM_PROFILE_FILE"] = str(llvm_profile_file)
    llvm_profile_dir = llvm_profile_file.parent
    source = Path(__file__).resolve().parent.parent
    build = source / "build"
    await cmd("cmake", "-G", "Ninja", "-B", build, "-S", source)
    try:
        llvm_profile_dir.rmdir()
    except FileNotFoundError:
        pass
    llvm_profile_dir.mkdir()
    await ninja(build, "check-rand", "regression")
    llvm_profile_files = map(str, filter(Path.is_file, llvm_profile_dir.iterdir()))
    instr_profile = llvm_profile_dir / "llvm-profile.profdata"
    await cmd(
        "llvm-profdata",
        "merge",
        "-sparse",
        *llvm_profile_files,
        f"--output={instr_profile}",
        timeout=600,
    )
    await cmd(
        "llvm-cov",
        "export",
        "build/unit-test",
        "--ignore-filename-regex=.*/(external|unit_tests)/.*",
        f"-instr-profile={instr_profile}",
        "--region-coverage-gt=2",
        "--format=lcov",
        output,
    )


if __name__ == "__main__":
    try:
        run(main(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
