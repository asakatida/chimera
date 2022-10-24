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
from os import chdir, environ
from pathlib import Path
from subprocess import DEVNULL, PIPE, CalledProcessError, TimeoutExpired, run
from sys import stderr

from requests import get


def cmd(*args: str) -> None:
    run(args, check=True, stderr=PIPE, stdout=DEVNULL)


def main() -> None:
    llvm_profile_file = Path(
        environ.get(
            "LLVM_PROFILE_FILE",
            Path(environ.get("LLVM_PROFILE_DIR", "/tmp/coverage"))
            / "llvm-profile.%c%p.profraw",
        )
    ).resolve()
    environ["LLVM_PROFILE_FILE"] = str(llvm_profile_file)
    llvm_profile_dir = llvm_profile_file.parent
    llvm = Path("/tmp/llvm.sh")
    llvm.write_bytes(get("https://apt.llvm.org/llvm.sh", allow_redirects=True).content)
    llvm.chmod(0o755)
    clang_version = environ.get("CLANG_VERSION", "15")
    cmd("sudo", str(llvm), clang_version)
    llvm.unlink()
    cmd("sudo", "apt-get", "install", "-y", "ninja-build")
    chdir(Path(__file__).parent.parent)
    cmd("cmake", "-G", "Ninja", "-B", "build", "-S", ".")
    try:
        llvm_profile_dir.rmdir()
    except FileNotFoundError:
        pass
    llvm_profile_dir.mkdir()
    cmd("tools/ninja.sh", "build", "check-rand", "regression")
    llvm_profile_files = map(str, filter(Path.is_file, llvm_profile_dir.iterdir()))
    instr_profile = llvm_profile_dir / "llvm-profile.profdata"
    cmd(
        f"llvm-profdata-{clang_version}",
        "merge",
        "-sparse",
        *llvm_profile_files,
        f"--output={instr_profile}",
    )
    cmd(
        f"llvm-cov-{clang_version}",
        "export",
        "build/unit-test",
        "--ignore-filename-regex=.*/(external|unit_tests)/.*",
        f"-instr-profile={instr_profile}",
        "--region-coverage-gt=2",
        "--format=lcov",
        str(llvm_profile_dir / "llvm-profile.lcov"),
    )


if __name__ == "__main__":
    try:
        main()
    except CalledProcessError as error:
        print(*error.cmd, file=stderr)
        print((error.stderr or b"").decode(), file=stderr)
        exit(error.returncode)
    except TimeoutExpired as error:
        print(*error.cmd, file=stderr)
        print((error.stderr or b"").decode(), file=stderr)
        exit(1)
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
