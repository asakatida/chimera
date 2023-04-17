# Copyright (c) 2023 Asa Katida <github@holomaplefeline.net>
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

"""git_rebase_all.py."""

from asyncio import run
from asyncio.subprocess import PIPE
from sys import argv, stderr
from typing import Iterable

from asyncio_cmd import ProcessError, cmd, cmd_env


async def git_cmd(*args: object) -> bytes:
    return await cmd("git", *args, err=PIPE, log=False, out=PIPE, timeout=60)


def splitlines(lines: bytes) -> Iterable[str]:
    return map(bytes.decode, filter(None, map(bytes.strip, lines.splitlines())))


async def git_rebase_all(*args: str) -> None:
    await git_cmd("fetch", "--all", "--prune")
    remote_branches = await git_cmd("branch", "-r")
    for remote_branch in splitlines(remote_branches):
        if "origin/HEAD" in remote_branch or remote_branch.startswith("dependabot/"):
            continue
        local_name = remote_branch.split("/", maxsplit=1)[-1]
        try:
            await git_cmd("branch", "--set-upstream-to", remote_branch, local_name)
        except ProcessError:
            await git_cmd("branch", local_name, remote_branch)
    local_branches = await git_cmd("branch")
    for local_branch in splitlines(local_branches):
        if local_branch.startswith("* "):
            local_branch = local_branch[1:].lstrip()
        try:
            await cmd("git", "rebase", "origin/HEAD", local_branch, err=None, out=None)
            continue
        except ProcessError:
            pass
        await cmd("bash", "-c", *args, err=None, out=None)
        await git_cmd("add", "--update")
        await cmd_env(
            "git",
            "rebase",
            "--continue",
            env={"EDITOR": "true"},
            err=None,
            out=None,
        )


if __name__ == "__main__":
    try:
        run(git_rebase_all(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
