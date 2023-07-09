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

from asyncio import gather, run
from os import chdir, environ
from pathlib import Path
from sys import argv

from asyncio_cmd import cmd, cmd_env, cmd_flog, main
from corpus_utils import regression
from ninja import ninja


async def codecov(llvm_profile_lcov: str) -> None:
    llvm_profile_file = Path(
        environ.get(
            "LLVM_PROFILE_FILE",
            Path(environ.get("LLVM_PROFILE_DIR", "/tmp/coverage"))
            / "llvm-profile.%m.profraw",
        )
    ).resolve()
    environ["LLVM_PROFILE_FILE"] = str(llvm_profile_file)
    llvm_profile_dir = llvm_profile_file.parent
    instr_profile = llvm_profile_dir / "llvm-profile.profdata"
    chdir(Path(__file__).parent.parent)
    await cmd_env(
        "cmake",
        "-G",
        "Ninja",
        "-B",
        "build",
        "-S",
        ".",
        env=dict(
            environ,
            CXXFLAGS=" ".join(
                (
                    "-fcoverage-mapping",
                    "-fprofile-instr-generate",
                    "-fsanitize-coverage=no-prune",
                    "-fsanitize-coverage=trace-cmp",
                    "-fsanitize-coverage=trace-div",
                    "-fsanitize-coverage=trace-gep",
                    "-fsanitize-coverage=trace-loads",
                    "-fsanitize-coverage=trace-stores",
                    "-mllvm",
                    "-runtime-counter-relocation",
                )
            ),
            LDFLAGS=" ".join(
                ("-Wno-unused-command-line-argument", environ.get("LDFLAGS", ""))
            ),
        ),
    )
    try:
        llvm_profile_dir.rmdir()
    except FileNotFoundError:
        pass
    llvm_profile_dir.mkdir()
    await ninja("build")
    await gather(ninja("build", "test"), regression("build"))
    await cmd(
        "llvm-profdata",
        "merge",
        "-sparse=true",
        *filter(Path.is_file, llvm_profile_dir.iterdir()),
        f"--output={instr_profile}",
    )
    await cmd_flog(
        "llvm-cov",
        "export",
        "build/unit-test",
        "--ignore-filename-regex=.*/(catch2|external|unit_tests)/.*",
        f"-instr-profile={instr_profile}",
        "--format=lcov",
        out=llvm_profile_lcov,
    )


if __name__ == "__main__":
    with main():
        run(codecov(*argv[1:]))
