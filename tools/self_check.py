from asyncio import gather, run
from re import compile
from sys import stderr
from typing import Awaitable

from asyncio_cmd import ProcessError, cmd
from g_ls_tree import g_ls_tree


async def main() -> None:
    files = await g_ls_tree("py")
    files_mypy = await g_ls_tree("py", exclude=compile(r"stdlib/|.*/stdlib/"))
    jobs: list[Awaitable[bytes]] = []
    if files:
        jobs.extend(
            (
                cmd(
                    "black",
                    "--check",
                    "--diff",
                    "--preview",
                    "--target-version",
                    "py311",
                    *files,
                    log=False,
                    stdout=None,
                    timeout=180,
                ),
                cmd(
                    "isort",
                    "--check-only",
                    "--python-version",
                    "auto",
                    *files,
                    log=False,
                    stdout=None,
                    timeout=180,
                ),
                cmd("pylama", *files, log=False, stdout=None, timeout=180),
            )
        )
    if files_mypy:
        jobs.append(
            cmd(
                "mypy",
                *files_mypy,
                log=False,
                stdout=None,
                timeout=180,
            )
        )
    if jobs:
        if errors := list(
            filter(
                lambda elem: isinstance(elem, Exception),
                await gather(*jobs, return_exceptions=True),
            )
        ):
            raise errors[0]


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
