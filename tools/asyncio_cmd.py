from asyncio import CancelledError, create_subprocess_exec, wait_for
from asyncio.subprocess import DEVNULL, PIPE, Process
from contextlib import contextmanager
from itertools import chain, islice, repeat, takewhile
from os import environ
from pathlib import Path
from sys import exc_info
from typing import Iterable, Iterator, Sequence, TextIO, TypeVar

from structlog import get_logger

IN_CI = environ.get("CI", "") == "true"
T = TypeVar("T")
ProcessInput = int | TextIO | None


class ProcessError(Exception):
    def __init__(
        self, cmd: Sequence[object], stdout: bytes, stderr: bytes, returncode: int
    ) -> None:
        super().__init__(f"{cmd} failed with {returncode}")
        self.cmd = cmd
        self.stderr = stderr.decode(errors="backslashreplace").strip()
        self.stdout = stdout.decode(errors="backslashreplace").strip()
        self.returncode = returncode

    def exit(self) -> None:
        get_logger().error(" ".join(map(str, self.cmd)))
        if self.stdout:
            for line in self.stdout.splitlines():
                get_logger().info(line)
        if self.stderr:
            for line in self.stderr.splitlines():
                get_logger().error(line)
        exit(self.returncode)


def chunks(iterable: Iterable[T], size: int) -> Iterable[list[T]]:
    return takewhile(
        lambda i: i, map(list, map(islice, repeat(iter(iterable)), repeat(size)))  # type: ignore
    )


def ci_args(*args: object, invert: bool = False) -> tuple[object, ...]:
    return args if IN_CI != invert else ()


def splitlines(lines: bytes) -> Iterable[str]:
    return map(bytes.decode, filter(None, map(bytes.strip, lines.splitlines())))


async def cmd(
    *args: object,
    err: ProcessInput = PIPE,
    log: bool = True,
    out: ProcessInput = PIPE,
    timeout: int | None = 20,
) -> bytes:
    if log:
        get_logger().info(f"+ {' '.join(map(str, args))}")
    proc = await create_subprocess_exec(*map(str, args), stderr=err, stdout=out)
    return await communicate(args, b"", proc, timeout)


async def cmd_check(*args: object, timeout: int | None = 20) -> Exception | None:
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


async def cmd_env(
    *args: object,
    env: dict[str, object] = {},
    err: ProcessInput = PIPE,
    out: ProcessInput = PIPE,
    timeout: int | None = 20,
) -> bytes:
    get_logger().info(f"+ {' '.join(map(str, args))}")
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
    *args: object, out: str | None = None, timeout: int | None = 20
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


async def cmd_no_timeout(*args: object) -> None:
    get_logger().info(f"+ {' '.join(map(str, args))}")
    proc = await create_subprocess_exec(*map(str, args))
    try:
        await proc.communicate()
    finally:
        await status(args, b"", b"", proc, None)


async def communicate(
    args: Sequence[object], err: bytes, proc: Process, timeout: int | None
) -> bytes:
    cmd_stderr = cmd_stdout = None
    try:
        cmd_stdout, cmd_stderr = await wait_for(proc.communicate(), timeout=timeout)
    finally:
        await status(
            args, (cmd_stdout or b""), err + (cmd_stderr or b""), proc, timeout
        )
    return cmd_stdout or b""


@contextmanager
def main() -> Iterator[None]:
    try:
        yield
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        get_logger().info("KeyboardInterrupt")


async def status(
    args: Sequence[object], out: bytes, err: bytes, proc: Process, timeout: int | None
) -> None:
    returncode = proc.returncode
    cmd_stderr = b""
    cmd_stdout = b""
    if returncode is None:
        proc.terminate()
        returncode = await proc.wait()
        if proc.stderr is not None:
            cmd_stderr = await proc.stderr.read()
        if proc.stdout is not None:
            cmd_stdout = await proc.stdout.read()
        cmd_stderr += (
            f"\nThe process was terminated by timeout {timeout} seconds".encode()
        )
    if returncode != 0:
        if not isinstance(exc_info()[1], CancelledError):
            raise ProcessError(
                args, out + cmd_stdout, err + cmd_stderr, returncode or 1
            )
