"""codecov.py"""

from asyncio import gather, run
from os import chdir, environ
from pathlib import Path
from sys import argv

from asyncio_cmd import cmd, cmd_flog, main
from chimera_utils import rmdir
from cmake_codecov import cmake_codecov
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
    await cmake_codecov("fuzzers", "unit-test")
    rmdir(llvm_profile_dir)
    llvm_profile_dir.mkdir(exist_ok=True, parents=True)
    await gather(
        ninja("build", "-k0", "test"),
        regression("build", return_exceptions=True),
        return_exceptions=True,
    )
    await cmd(
        "llvm-profdata",
        "merge",
        "-sparse=true",
        *(path for path in llvm_profile_dir.iterdir() if path.is_file()),
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
