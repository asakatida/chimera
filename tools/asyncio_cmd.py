from asyncio import create_subprocess_exec, wait_for
from asyncio.subprocess import DEVNULL, PIPE
from itertools import islice, repeat, takewhile
from sys import stderr
from typing import Iterable, Optional, TextIO, TypeVar, Union

T = TypeVar("T")


class ProcessError(Exception):
    def __init__(self, cmd: tuple[object, ...], stderr: bytes, returncode: int) -> None:
        super().__init__(f"{cmd} failed with {returncode}")
        self.cmd = cmd
        self.stderr = stderr.decode().strip()
        self.returncode = returncode

    def exit(self) -> None:
        print(*self.cmd, file=stderr)
        print(self.stderr, file=stderr)
        exit(self.returncode)


def chunks(iterable: Iterable[T], size: int) -> Iterable[list[T]]:
    return takewhile(
        lambda i: i, map(list, map(islice, repeat(iterable), repeat(size)))  # type: ignore
    )


async def cmd(
    *args: object,
    log: bool = True,
    stdout: Optional[Union[int, TextIO]] = DEVNULL,
    timeout: int = 20,
) -> bytes:
    if log:
        print("+", *args, file=stderr)
    proc = await create_subprocess_exec(*map(str, args), stderr=PIPE, stdout=stdout)
    cmd_stderr = b""
    try:
        cmd_stdout, cmd_stderr = await wait_for(proc.communicate(), timeout=timeout)
    finally:
        returncode = proc.returncode
        if returncode is None:
            proc.terminate()
            returncode = await proc.wait()
            if proc.stderr is not None:
                cmd_stderr += await proc.stderr.read()
            cmd_stderr += (
                f"\nThe process was terminated by timeout {timeout} seconds".encode()
            )
        if returncode != 0:
            raise ProcessError(tuple(map(str, args)), cmd_stderr, returncode or 1)
    return cmd_stdout


async def cmd_env(
    env: dict[str, object],
    *args: object,
    stdout: Optional[int] = DEVNULL,
    timeout: int = 20,
) -> bytes:
    print("+", *args, file=stderr)
    proc = await create_subprocess_exec(
        *map(str, args),
        env=dict(zip(env.keys(), map(str, env.values()))),
        stderr=PIPE,
        stdout=stdout,
    )
    cmd_stderr = b""
    try:
        cmd_stdout, cmd_stderr = await wait_for(proc.communicate(), timeout=timeout)
    finally:
        returncode = proc.returncode
        if returncode is None:
            proc.terminate()
            returncode = await proc.wait()
            if proc.stderr is not None:
                cmd_stderr += await proc.stderr.read()
            cmd_stderr += (
                f"\nThe process was terminated by timeout {timeout} seconds".encode()
            )
        if returncode != 0:
            raise ProcessError(tuple(map(str, args)), cmd_stderr, returncode or 1)
    return cmd_stdout


async def cmd_no_timeout(*args: object) -> None:
    print("+", *args, file=stderr)
    proc = await create_subprocess_exec(*map(str, args))
    cmd_stderr = b""
    try:
        await proc.communicate()
    finally:
        returncode = proc.returncode
        if returncode is None:
            proc.terminate()
            returncode = await proc.wait()
        if returncode != 0:
            raise ProcessError(tuple(map(str, args)), cmd_stderr, returncode or 1)
