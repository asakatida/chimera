"""git_rebase_all.py."""

from asyncio import run
from asyncio.subprocess import PIPE
from itertools import combinations
from math import factorial
from os import environ
from sys import argv
from typing import Sequence

from asyncio_cmd import ProcessError, cmd, main, splitlines
from corpus_utils import c_tqdm
from structlog import get_logger


async def git_cmd(*args: object, out: int | None = None) -> bytes:
    return await cmd("git", *args, out=out, log=False)


async def set_upstream(*remote_branches: str) -> None:
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
    remote_branches: Sequence[str],
    local_branches: Sequence[str],
    disable_bars: bool | None,
) -> None:
    branch_graph: dict[str, set[str]] = {}
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
    for remote in sorted(
        {branch for branch in branch_graph.keys()}.intersection(remote_branches)
    ):
        for local in sorted(
            {branch for branch in branch_graph.get(remote, [])}.difference(
                remote_branches
            )
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
        await get_logger().ainfo(f"{remote} -> {' '.join(sorted(branches))}")


async def git_rebase_all(*args: str, disable_bars: bool | None) -> None:
    remote_branches = [
        branch
        for branch in splitlines(await git_cmd("branch", "-r", out=PIPE))
        if not branch.startswith("origin/HEAD -> ")
    ]
    await set_upstream(*remote_branches)
    local_branches = [
        (local_branch[1:].lstrip() if local_branch.startswith("* ") else local_branch)
        for local_branch in splitlines(await git_cmd("branch", out=PIPE))
    ]
    for local_branch in c_tqdm(local_branches, "Rebase branches", disable_bars):
        if (
            await git_cmd(
                "rev-list",
                "--cherry-pick",
                "--count",
                "--right-only",
                f"{local_branch}...origin/stable",
                out=PIPE,
            )
        ).strip() == b"0":
            continue
        try:
            await cmd("git", "rebase", "origin/stable", local_branch, log=False)
            continue
        except ProcessError:
            pass
        await git_cmd("submodule", "update", "--init", "--recursive")
        await cmd("sh", "-c", *args, log=False)
        await git_cmd("add", "--update")
        await cmd("git", "rebase", "--continue", log=False)
    await report_branch_graph(
        [remote_branch.split("/", maxsplit=1)[1] for remote_branch in remote_branches],
        local_branches,
        disable_bars,
    )


if __name__ == "__main__":
    with main():
        environ["EDITOR"] = "true"
        run(git_rebase_all(*argv[1:], disable_bars=None))
