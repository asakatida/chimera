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
from itertools import chain, product, repeat
from pathlib import Path
from re import MULTILINE, compile
from typing import Iterable, TypeVar

from asyncio_as_completed import as_completed
from asyncio_cmd import chunks, cmd, cmd_check, cmd_flog, splitlines
from tqdm import tqdm

CONFLICT = compile(rb"^(?:(?:<{7,8}|>{7,8})(?:\s.+)?|={7,8})$\s?", MULTILINE)
DIRECTORIES = ("corpus", "crashes")
SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"
CORPUS.mkdir(parents=True, exist_ok=True)
LENGTH = 14

T = TypeVar("T")


def bucket(path: Path) -> Path:
    return FUZZ / path.relative_to(FUZZ).parts[0]


def c_tqdm(
    iterable: Iterable[T], desc: str, disable: bool, total: float | None = None
) -> Iterable[T]:
    return tqdm(iterable, desc=desc, disable=disable, total=total, unit_scale=True)


def conflicts_one(file: Path) -> None:
    set(
        map(
            lambda section: (file.parent / sha256(section).hexdigest()).write_bytes(
                section
            ),
            filter(None, CONFLICT.split(file.read_bytes())),
        )
    )
    file.unlink()


def conflicts(fuzz: Iterable[Path]) -> None:
    set(
        map(
            conflicts_one, filter(lambda file: CONFLICT.search(file.read_bytes()), fuzz)
        )
    )


async def corpus_creations(*paths: str) -> dict[bytes, list[str]]:
    return dict(
        map(
            lambda pair: pair[1:],
            sorted(
                filter(
                    lambda pair: pair[2],
                    map(
                        lambda match: (
                            match["date"],
                            match["sha"],
                            list(
                                filter(
                                    lambda line: any(
                                        map(lambda path: path in line, paths)  # type: ignore
                                    )
                                    and not Path(line).exists(),  # type: ignore
                                    splitlines(match["paths"]),
                                )
                            ),
                        ),
                        compile(
                            rb"commit:(?P<date>.+):(?P<sha>.+)(?P<paths>(?:\s+(?!commit:).+)+)"
                        ).finditer(
                            await cmd(
                                "git",
                                "log",
                                "--all",
                                "--date=iso",
                                "--diff-filter=A",
                                "--name-only",
                                "--pretty=format:commit:%cd:%h",
                                "^HEAD",
                                "--",
                                *paths,
                                out=PIPE,
                            )
                        ),
                    ),
                )
            ),
        )
    )


async def corpus_merge(path: Path) -> list[Exception]:
    if errors := await fuzz_test(
        "-merge=1",
        "-reduce_inputs=1",
        "-shrink=1",
        CORPUS,
        *filter(
            Path.is_dir,
            chain.from_iterable(map(lambda path: path.rglob("*"), (CORPUS, path))),
        ),
    ):
        return errors
    corpus_trim(disable_bars=True)
    return []


def corpus_trim_one(fuzz: Iterable[Path], disable_bars: bool) -> None:
    global LENGTH
    for file in c_tqdm(fuzz, "Corpus rehash", disable_bars):
        src_sha = sha(file)
        name = src_sha[:LENGTH]
        if name == (file.parent.name + file.name):
            continue
        sha_bucket, name = name[:2], name[2:]
        if set(
            map(
                sha,
                filter(
                    Path.exists,
                    map(FUZZ.joinpath, DIRECTORIES, repeat(sha_bucket), repeat(name)),
                ),
            )
        ).difference({src_sha}):
            LENGTH += 1
            raise Increment(
                f"Collision found, update corpus_trim.py `LENGTH`: {LENGTH}"
            )
        new_file = bucket(file) / sha_bucket / name
        new_file.parent.mkdir(parents=True, exist_ok=True)
        file.rename(new_file)
    for file in filter(
        Path.exists,
        map(
            CRASHES.joinpath,
            map(
                lambda path: path.relative_to(bucket(path)),
                filter(Path.is_file, CORPUS.rglob("*")),
            ),
        ),
    ):
        file.unlink()


def corpus_trim(disable_bars: bool = False) -> None:
    conflicts(gather_paths())
    CRASHES.mkdir(parents=True, exist_ok=True)
    for file in chain.from_iterable(
        map(SOURCE.rglob, ("crash-*", "leak-*", "timeout-*"))
    ):
        file.rename(CRASHES / sha(file))
    while True:
        try:
            corpus_trim_one(gather_paths(), disable_bars)
        except Increment:
            if LENGTH > 32:
                raise
            continue
        break


@cache
def fuzz_star(build: Path = SOURCE) -> tuple[Path, ...]:
    return tuple(
        filter(
            lambda path: path.stat().st_mode & 0o110,
            filter(Path.is_file, build.rglob("fuzz-*")),
        )
    )


async def fuzz_test(*args: object) -> list[Exception]:
    if not fuzz_star():
        raise FileNotFoundError("No fuzz targets built")
    return list(
        filter(
            None,
            await as_completed(
                map(lambda cmd, args: cmd_check(cmd, *args), fuzz_star(), repeat(args))
            ),
        )
    )


def gather_paths() -> Iterable[Path]:
    return filter(
        Path.is_file,
        chain.from_iterable(
            map(Path.rglob, map(FUZZ.joinpath, DIRECTORIES), repeat("*"))  # type: ignore
        ),
    )


class Increment(Exception):
    pass


async def regression_one(fuzzer: Path, chunk: list[Path]) -> None:
    await cmd_flog(fuzzer, *chunk)


async def regression(build: str, fuzzer: str = "", corpus: str = "") -> None:
    fuzzers = (Path(build) / f"fuzz-{fuzzer}",) if fuzzer else fuzz_star(Path(build))
    await as_completed(
        map(
            lambda args: regression_one(*args),
            product(
                fuzzers,
                chunks(
                    filter(
                        Path.is_file, (Path(corpus) if corpus else CORPUS).rglob("*")
                    ),
                    64,
                ),
            ),
        ),
        cancel=True,
    )


def sha(path: Path) -> str:
    return sha256(path.read_bytes()).hexdigest()
