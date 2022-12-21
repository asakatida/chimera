from asyncio import run
from asyncio.subprocess import PIPE
from re import compile
from sys import stderr
from typing import Awaitable

from asyncio_as_completed import as_completed, raise_errors
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
                    stdout=PIPE,
                    timeout=300,
                ),
                cmd(
                    "isort",
                    "--check-only",
                    "--python-version",
                    "auto",
                    *files,
                    log=False,
                    stdout=PIPE,
                    timeout=300,
                ),
                cmd("pylama", *files, log=False, stdout=PIPE, timeout=300),
            )
        )
    if files_mypy:
        jobs.append(
            cmd(
                "mypy",
                *files_mypy,
                log=False,
                stdout=PIPE,
                timeout=300,
            )
        )
    if jobs:
        results = await as_completed(jobs)
        raise_errors("Warning: got multiple errors:", results)
        print(
            *map(
                bytes.decode,
                filter(lambda elem: isinstance(elem, bytes), results),
            ),
            sep="\n",
        )


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
