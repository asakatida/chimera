from asyncio import CancelledError, create_subprocess_exec, wait_for
from asyncio.subprocess import DEVNULL, Process
from itertools import chain, islice, repeat, takewhile
from os import environ
from pathlib import Path
from sys import exc_info, stderr
from time import monotonic_ns
from typing import Iterable, Optional, Sequence, TextIO, TypeVar, Union

T = TypeVar("T")
ProcessInput = Optional[Union[int, TextIO]]


class TimeIt:
    def __init__(self, callable: T) -> None:
        self.callable = callable

    async def __call__(self, *args: object, **kwds: object) -> bytes:
        if not kwds.get("log", True):
            return await self.callable(*args, **kwds)  # type: ignore
        self.args = args
        with self:
            return await self.callable(*args, **kwds)  # type: ignore

    def __enter__(self) -> None:
        self.start = monotonic_ns()

    def __exit__(self, *_: object) -> None:
        milliseconds, nanoseconds = divmod(monotonic_ns() - self.start, 1_000_000)
        seconds, milliseconds = divmod(milliseconds, 1_000)
        minutes, seconds = divmod(seconds, 60)
        hours, minutes = divmod(minutes, 60)
        print(
            "-",
            "took",
            hours,
            "h",
            minutes,
            "m",
            seconds,
            "s",
            milliseconds,
            "ms",
            nanoseconds,
            "ns",
            *self.args,
            file=stderr,
        )


class ProcessError(Exception):
    def __init__(self, cmd: Sequence[object], stderr: bytes, returncode: int) -> None:
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
        lambda i: i, map(list, map(islice, repeat(iter(iterable)), repeat(size)))  # type: ignore
    )


@TimeIt
async def cmd(
    *args: object,
    err: ProcessInput,
    log: bool = True,
    out: ProcessInput,
    timeout: int = 20,
) -> bytes:
    if log:
        print("+", *args, file=stderr)
    proc = await create_subprocess_exec(*map(str, args), stderr=err, stdout=out)
    return await communicate(args, b"", proc, timeout)


async def cmd_check(*args: object, timeout: int = 20) -> Optional[Exception]:
    try:
        proc = await create_subprocess_exec(
            *map(str, args), stderr=DEVNULL, stdout=DEVNULL
        )
        await communicate(args, b"", proc, timeout)
    except CancelledError:
        raise
    except Exception as error:
        return error
    return None


@TimeIt
async def cmd_env(
    *args: object,
    env: dict[str, object] = {},
    err: ProcessInput,
    out: ProcessInput,
    timeout: int = 20,
) -> bytes:
    print("+", *args, file=stderr)
    proc = await create_subprocess_exec(
        *map(str, args),
        env=dict(
            chain(
                filter(lambda item: item[0] in ("PATH", "PWD"), environ.items()),
                zip(env.keys(), map(str, env.values())),
            )
        ),
        stderr=err,
        stdout=out,
    )
    return await communicate(args, b"", proc, timeout)


async def cmd_flog(
    *args: object, out: Optional[str] = None, timeout: int = 20
) -> bytes:
    if out is None:
        proc = await create_subprocess_exec(
            *map(str, args), stderr=DEVNULL, stdout=DEVNULL
        )
        return await communicate(args, b"logs in /dev/null\n", proc, timeout)
    with Path(out).open("ab") as ostream:
        proc = await create_subprocess_exec(
            *map(str, args), stderr=ostream, stdout=ostream
        )
        return await communicate(args, f"logs in {out}\n".encode(), proc, timeout)


@TimeIt
async def cmd_no_timeout(*args: object) -> None:
    print("+", *args, file=stderr)
    proc = await create_subprocess_exec(*map(str, args))
    try:
        await proc.communicate()
    finally:
        await status(args, b"", proc, -1)


async def communicate(
    args: Sequence[object], err: bytes, proc: Process, timeout: int
) -> bytes:
    cmd_stderr = b""
    try:
        cmd_stdout, cmd_stderr = await wait_for(proc.communicate(), timeout=timeout)
    finally:
        await status(args, err + (cmd_stderr or b""), proc, timeout)
    return cmd_stdout or b""


async def status(
    args: Sequence[object], err: bytes, proc: Process, timeout: int
) -> None:
    returncode = proc.returncode
    cmd_stderr = b""
    if returncode is None:
        proc.terminate()
        returncode = await proc.wait()
        if proc.stderr is not None:
            cmd_stderr += await proc.stderr.read()
        cmd_stderr += (
            f"\nThe process was terminated by timeout {timeout} seconds".encode()
        )
    if returncode != 0:
        if not isinstance(exc_info()[1], CancelledError):
            raise ProcessError(args, err + cmd_stderr, returncode or 1)
