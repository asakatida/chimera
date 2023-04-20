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
from asyncio.subprocess import DEVNULL, PIPE
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd, cmd_check
from corpus_retest import corpus_retest
from corpus_trim import corpus_trim
from ninja import ninja
from tqdm import tqdm


async def git_cmd(*args: object) -> None:
    await cmd("git", *args, err=PIPE, log=False, out=DEVNULL)


async def git_cmd_remote(*args: object) -> None:
    await cmd("git", *args, err=None, out=None, timeout=600)


async def git_restore(sha: str, *paths: object) -> None:
    try:
        await git_cmd("restore", "--source", sha, "--staged", *paths)
    except ProcessError:
        return
    try:
        await git_cmd("restore", "--worktree", *paths)
    except ProcessError:
        pass
    corpus_trim()
    await git_cmd("add", *paths)
    await git_cmd("commit", "--allow-empty", "--amend", "--no-edit")


async def corpus_gather(*paths: str) -> None:
    await git_cmd_remote("add", *paths)
    await git_cmd_remote("commit", "--allow-empty", "-m", "WIP")
    git_log = await cmd(
        "git",
        "log",
        "--all",
        "--format=%h",
        "^origin/HEAD",
        "--",
        *paths,
        err=None,
        out=PIPE
    )
    for sha in tqdm(
        map(bytes.decode, filter(None, map(bytes.strip, git_log.splitlines()))),
        desc="Refs",
        unit_scale=True,
    ):
        await git_restore(sha, *paths)
    await git_restore("origin/HEAD", *paths)
    await cmd("git", "reset", "HEAD^", err=DEVNULL, out=DEVNULL, timeout=900)


async def main(ref: str) -> None:
    await git_cmd("config", "--local", "user.email", "email")
    await git_cmd("config", "--local", "user.name", "name")
    await cmd("rustup", "default", "stable", err=None, out=None)
    await cmd("cmake", "-GNinja", "-B", "build", "-S", ".", err=None, out=None)
    await ninja("build")
    path = "unit_tests/fuzz/corpus"
    if ref == "refs/heads/stable":
        await git_cmd("remote", "set-head", "origin", "--auto")
        await corpus_gather(path)
    await corpus_retest("build")
    await ninja("build", "regression")
    for opt in ("--global", "--local", "--system", "--worktree"):
        await cmd_check("git", "config", opt, "--unset-all", "user.email")
        await cmd_check("git", "config", opt, "--unset-all", "user.name")
    corpus_trim()


if __name__ == "__main__":
    try:
        run(main(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
