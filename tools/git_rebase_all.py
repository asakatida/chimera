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
from itertools import combinations
from math import factorial
from sys import argv

from asyncio_cmd import ProcessError, cmd, cmd_env, main, splitlines
from corpus_utils import c_tqdm
from structlog import get_logger


async def git_cmd(*args: object, out: int | None = None) -> bytes:
    return await cmd("git", *args, out=out, log=False)


async def set_upstream(remote_branches: list[str]) -> None:
    for remote_branch in remote_branches:
        if remote_branch.startswith("origin/dependabot/"):
            continue
        local_name = remote_branch.split("/", maxsplit=1)[1]
        try:
            await git_cmd("branch", "--set-upstream-to", remote_branch, local_name)
        except ProcessError:
            await git_cmd("branch", local_name, remote_branch)


async def git_diff(*args: str) -> bool:
    try:
        await git_cmd("diff", "--exit-code", "--quiet", *args)
    except ProcessError:
        return False
    return True


async def report_branch_graph(
    remote_branches: list[str], local_branches: list[str], disable_bars: bool
) -> None:
    branch_graph: dict[str, set[str]] = dict()
    for left, right in c_tqdm(
        combinations(local_branches, 2),
        "Gather common branches",
        disable_bars,
        factorial(len(local_branches)) // 2 // factorial(len(local_branches) - 2),
    ):
        if not await git_diff(left, right):
            continue
        branch_graph.setdefault(left, set())
        branch_graph.setdefault(right, set())
        branch_graph[left].add(right)
        branch_graph[right].add(left)
    for remote in sorted(set(branch_graph.keys()).intersection(remote_branches)):
        for local in sorted(
            set(branch_graph.get(remote, [])).difference(remote_branches)
        ):
            if local in branch_graph:
                branch_graph[remote] |= branch_graph[local]
                del branch_graph[local]
            for branches in branch_graph.values():
                if local in branches:
                    branches |= branch_graph[remote]
                if remote in branches:
                    branches.remove(remote)
    for remote, branches in branch_graph.items():
        get_logger().info(f"{remote} -> {' '.join(sorted(branches))}")


async def git_rebase_all(*args: str, disable_bars: bool) -> None:
    await git_cmd("fetch", "--all", "--prune")
    remote_branches = list(
        filter(
            lambda branch: not branch.startswith("origin/HEAD -> "),
            splitlines(await git_cmd("branch", "-r", out=PIPE)),
        )
    )
    await set_upstream(remote_branches)
    local_branches = list(
        map(
            lambda local_branch: (
                local_branch[1:].lstrip()
                if local_branch.startswith("* ")
                else local_branch
            ),
            splitlines(await git_cmd("branch", out=PIPE)),
        )
    )
    for local_branch in c_tqdm(local_branches, "Rebase branches", disable_bars):
        try:
            await cmd("git", "rebase", "origin/stable", local_branch, log=False)
            continue
        except ProcessError:
            pass
        await git_cmd("submodule", "update", "--init", "--recursive")
        await cmd("sh", "-c", *args, log=False)
        await git_cmd("add", "--update")
        await cmd_env("git", "rebase", "--continue", env={"EDITOR": "true"}, log=False)
    await report_branch_graph(
        list(
            map(
                lambda remote_branch: remote_branch.split("/", maxsplit=1)[1],
                remote_branches,
            )
        ),
        local_branches,
        disable_bars,
    )


if __name__ == "__main__":
    with main():
        run(git_rebase_all(*argv[1:], disable_bars=False))
