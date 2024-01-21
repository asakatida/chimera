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

"""corpus_utils.py"""

from asyncio.subprocess import PIPE
from functools import cache
from hashlib import sha256
from os import environ
from pathlib import Path
from re import MULTILINE, compile
from typing import Iterable, TypeVar

from asyncio_as_completed import as_completed
from asyncio_cmd import chunks, cmd, cmd_check, cmd_flog, splitlines
from chimera_utils import IN_CI, rmdir
from structlog import get_logger
from tqdm import tqdm

CONFLICT = compile(
    rb"^(?:<{8}(?:\s+HEAD:\S+)?|>{8}(?:\s+\w+\s+\([^\)]+\):\S+)?|={8})$\s?", MULTILINE
)
DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CORPUS_ORIGINAL = FUZZ / "corpus_original"
CRASHES = FUZZ / "crashes"
CORPUS.mkdir(exist_ok=True, parents=True)
LENGTH = 14

T = TypeVar("T")


class Increment(Exception):
    pass


def bucket(path: Path) -> Path:
    return FUZZ / path.relative_to(FUZZ).parts[0]


def c_tqdm(
    iterable: Iterable[T], desc: str, disable: bool | None, total: float | None = None
) -> Iterable[T]:
    if IN_CI and disable is not False:
        return iterable
    return tqdm(iterable, desc=desc, disable=disable, total=total, unit_scale=True)


def conflicts_one(file: Path) -> None:
    for section in CONFLICT.split(file.read_bytes()):
        if section:
            (file.parent / sha256(section).hexdigest()).write_bytes(section)
    file.unlink()


def conflicts(fuzz: Iterable[Path]) -> None:
    for file in (file for file in fuzz if CONFLICT.search(file.read_bytes())):
        conflicts_one(file)


async def corpus_cat(sha: str) -> tuple[str, bytes]:
    return (sha, await cmd("git", "cat-file", "-p", sha, log=False, out=PIPE))


async def corpus_creations(
    *paths: str,
    base_reference: str = environ.get("BASE_REF", "^origin/stable"),
    disable_bars: bool | None,
) -> Iterable[tuple[str, list[str]]]:
    return (
        pair
        for pair in (
            (
                match["sha"].decode(),
                [
                    line
                    for line in splitlines(match["paths"])
                    if any(line.startswith(path) for path in paths)
                ],
            )
            for match in c_tqdm(
                compile(
                    rb"commit:.+:(?P<sha>.+)(?P<paths>(?:\s+(?!commit:).+)+)"
                ).finditer(
                    await cmd(
                        "git",
                        "log",
                        *("--all",) if base_reference.startswith("^") else (),
                        "--date=iso",
                        "--diff-filter=A",
                        "--name-only",
                        "--pretty=format:commit:%cd:%h",
                        base_reference,
                        "--",
                        *paths,
                        out=PIPE,
                    )
                ),
                "Commits",
                disable_bars,
            )
        )
        if pair[1]
    )


async def corpus_merge(disable_bars: bool | None) -> None:
    rmdir(CORPUS_ORIGINAL)
    CORPUS.rename(CORPUS_ORIGINAL)
    CORPUS.mkdir(exist_ok=True, parents=True)
    if errors := await fuzz_test(
        "-merge=1",
        "-reduce_inputs=1",
        "-shrink=1",
        CORPUS,
        *(path for path in CORPUS_ORIGINAL.rglob("*") if path.is_dir()),
    ):
        error = errors.pop()
        if errors:
            for error in errors:
                await get_logger().aerror(f"Extra Error: {error}")
        raise error
    rmdir(CORPUS_ORIGINAL)
    corpus_trim(disable_bars=disable_bars)


async def corpus_objects(
    *paths: str, disable_bars: bool | None, exclude: set[str] = set()
) -> list[tuple[str, bytes]]:
    return await as_completed(
        corpus_cat(sha)
        for sha in c_tqdm(
            {
                line
                for lines in (
                    lines
                    for completed in await as_completed(
                        as_completed(
                            cmd(
                                "git",
                                "ls-tree",
                                "--full-tree",
                                "--object-only",
                                "-r",
                                item[0],
                                *chunk,
                                log=False,
                                out=PIPE,
                            )
                            for chunk in chunks(item[1], 4096)
                        )
                        for item in await corpus_creations(
                            *paths, disable_bars=disable_bars
                        )
                    )
                    for lines in completed
                )
                for line in splitlines(lines)
            }
            - exclude,
            "Files",
            disable_bars,
        )
    )


def corpus_trim_one(fuzz: Iterable[Path], disable_bars: bool | None) -> None:
    global LENGTH
    for file in c_tqdm(fuzz, "Corpus rehash", disable_bars):
        src_sha = sha(file)
        name = src_sha[:LENGTH]
        if name == (file.parent.name + file.name):
            continue
        sha_bucket, name = name[:2], name[2:]
        if {
            sha(path)
            for path in (
                FUZZ / directory / sha_bucket / name for directory in DIRECTORIES
            )
            if path.exists()
        }.difference({src_sha}):
            raise Increment(
                f"Collision found, update corpus_trim.py `LENGTH`: {LENGTH}"
            )
        new_file = bucket(file) / sha_bucket / name
        new_file.parent.mkdir(exist_ok=True, parents=True)
        file.rename(new_file)
    for file in (
        CORPUS / path.relative_to(bucket(path))
        for path in CRASHES.rglob("*")
        if path.is_file()
    ):
        file.unlink(missing_ok=True)


def corpus_trim(disable_bars: bool | None) -> None:
    global LENGTH
    conflicts(gather_paths())
    CRASHES.mkdir(exist_ok=True, parents=True)
    for file in (
        file
        for glob in ("crash-*", "leak-*", "timeout-*")
        for file in SOURCE.rglob(glob)
    ):
        file.rename(CRASHES / sha(file))
    while True:
        try:
            corpus_trim_one(gather_paths(), disable_bars)
        except Increment:
            if LENGTH > 32:
                raise
            LENGTH += 1
            continue
        break


@cache
def fuzz_star(build: Path = SOURCE) -> list[Path]:
    return [
        path
        for path in build.rglob("fuzz-*")
        if path.is_file() and path.stat().st_mode & 0o110
    ]


async def fuzz_test(*args: object) -> list[Exception]:
    if not fuzz_star():
        raise FileNotFoundError("No fuzz targets built")
    return [
        exc
        for exc in await as_completed(cmd_check(fuzz, *args) for fuzz in fuzz_star())
        if exc is not None
    ]


def gather_paths() -> Iterable[Path]:
    return (
        path
        for directory in DIRECTORIES
        for path in (FUZZ / directory).rglob("*")
        if path.is_file()
    )


async def regression(build: str, fuzzer: str = "") -> None:
    fuzzers = (Path(build) / f"fuzz-{fuzzer}",) if fuzzer else fuzz_star(Path(build))
    await as_completed(
        cmd_flog(fuzz, *args)
        for args in (
            {
                path
                for path in corpus.glob("*")
                if path.is_file() and path.name != ".done"
            }
            for corpus in CORPUS.glob("*")
            if not (corpus / ".done").exists()
        )
        for fuzz in fuzzers
        if args
    )


def sha(path: Path) -> str:
    return sha256(path.read_bytes()).hexdigest()
