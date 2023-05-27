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

"""corpus_gather.py"""

from asyncio import run
from asyncio.subprocess import DEVNULL
from sys import argv

from asyncio_cmd import ProcessError, cmd, cmd_check, main, splitlines
from corpus_retest import corpus_retest
from corpus_trim import corpus_trim
from corpus_utils import regression
from ninja import ninja
from tqdm import tqdm


async def git_cmd(*args: object) -> None:
    await cmd("git", *args, log=False, out=DEVNULL)


async def git_cmd_remote(*args: object) -> None:
    await cmd("git", *args, timeout=600)


async def git_restore(sha: str, *paths: object, disable_bars: bool) -> None:
    try:
        await git_cmd("restore", "--source", sha, *paths)
    except ProcessError:
        return
    deleted = tuple(
        filter(
            lambda line: line.startswith(" D "),
            splitlines(await cmd("git", "status", "--porcelain", log=False)),
        )
    )
    if deleted:
        await git_cmd("restore", *map(lambda line: line[-1], map(str.split, deleted)))
    corpus_trim(disable_bars=disable_bars)
    await git_cmd("add", *paths)
    await git_cmd("commit", "--allow-empty", "--amend", "--no-edit")


async def corpus_gather(*paths: str, disable_bars: bool) -> None:
    await git_cmd_remote("add", *paths)
    await git_cmd_remote("commit", "--allow-empty", "-m", "WIP")
    for sha in tqdm(
        list(
            splitlines(
                await cmd(
                    "git", "log", "--all", "--format=%h", "^origin/HEAD", "--", *paths
                )
            )
        ),
        desc="Refs",
        disable=disable_bars,
        unit_scale=True,
    ):
        await git_restore(sha, *paths, disable_bars=True)
    await git_restore("origin/HEAD", *paths, disable_bars=disable_bars)
    await cmd("git", "reset", "HEAD^", err=DEVNULL, out=DEVNULL, timeout=900)


async def corpus_gather_stable(disable_bars: bool) -> None:
    path = "unit_tests/fuzz/corpus"
    await git_cmd("config", "--local", "user.email", "email")
    await git_cmd("config", "--local", "user.name", "name")
    await git_cmd("remote", "set-head", "origin", "--auto")
    await corpus_gather(path, disable_bars=disable_bars)
    await corpus_retest("build")
    await regression("build")
    for opt in ("--global", "--local", "--system", "--worktree"):
        await cmd_check("git", "config", opt, "--unset-all", "user.email")
        await cmd_check("git", "config", opt, "--unset-all", "user.name")
    corpus_trim(disable_bars=disable_bars)


async def corpus_gather_main(ref: str, disable_bars: bool) -> None:
    await cmd("cmake", "-GNinja", "-B", "build", "-S", ".")
    await ninja("build")
    await regression("build")
    if ref == "refs/heads/stable":
        await corpus_gather_stable(disable_bars=disable_bars)


def main_(ref: str) -> None:
    run(corpus_gather_main(ref, disable_bars=False))


if __name__ == "__main__":
    with main():
        main_(*argv[1:])
