from asyncio import CancelledError, create_subprocess_exec
from asyncio.subprocess import DEVNULL, PIPE, Process
from contextlib import contextmanager
from itertools import islice
from os import environ
from pathlib import Path
from sys import exc_info
from typing import Iterable, Iterator, TypeVar

from chimera_utils import IN_CI
from structlog import get_logger

T = TypeVar("T")
ProcessInput = int | None


class ProcessError(Exception):
    def __init__(
        self, *cmd: str, stdout: bytes, stderr: bytes, returncode: int
    ) -> None:
        super().__init__(f"{cmd} failed with {returncode}")
        self.cmd = cmd
        self.stderr = stderr.decode(errors="backslashreplace").strip()
        self.stdout = stdout.decode(errors="backslashreplace").strip()
        self.returncode = returncode

    def exit(self) -> None:
        get_logger().error(" ".join(self.cmd))
        if self.stdout:
            for line in self.stdout.splitlines():
                get_logger().info(line)
        if self.stderr:
            for line in self.stderr.splitlines():
                get_logger().error(line)
        exit(self.returncode)


def chunks(iterable: Iterable[T], size: int) -> Iterable[list[T]]:
    it = iter(iterable)
    while slc := [elem for elem in islice(it, size)]:
        yield slc


def ci_args(*args: object, invert: bool = False) -> tuple[object, ...]:
    return args if IN_CI != invert else ()


def splitlines(lines: bytes) -> Iterable[str]:
    return (
        line.decode() for line in (line.strip() for line in lines.splitlines()) if line
    )


async def _cmd(
    *args: str,
    err: ProcessInput = PIPE,
    input: bytes | None = None,
    log: bool = True,
    out: ProcessInput = None,
) -> bytes:
    if log:
        get_logger().info(f"+ {' '.join(args)}")
    proc = await create_subprocess_exec(
        *args, stderr=err, stdin=(PIPE if input else None), stdout=out
    )
    return await communicate(*args, err=b"", proc=proc, input=input)


async def cmd(
    *args: object,
    err: ProcessInput = PIPE,
    input: bytes | None = None,
    log: bool = True,
    out: ProcessInput = None,
) -> bytes:
    return await _cmd(
        *(str(arg) for arg in args), err=err, input=input, log=log, out=out
    )


async def _cmd_check(*args: str) -> Exception | None:
    try:
        proc = await create_subprocess_exec(*args, stderr=DEVNULL, stdout=DEVNULL)
        await communicate(*args, err=b"", proc=proc)
    except ProcessError as error:
        return error
    return None


async def cmd_check(*args: object) -> Exception | None:
    return await _cmd_check(*(str(arg) for arg in args))


async def _cmd_env(
    *args: str,
    env: dict[str, object] = {},
    err: ProcessInput = PIPE,
    log: bool = True,
    out: ProcessInput = None,
) -> bytes:
    if log:
        get_logger().info(f"+ {' '.join(str(arg) for arg in args)}")
    proc = await create_subprocess_exec(
        *(str(arg) for arg in args),
        env=dict(
            {"PATH": environ.get("PATH", ""), "PWD": str(Path.cwd())},
            **{
                key: value for key, value in zip(env.keys(), (str(arg) for arg in args))
            },
        ),
        stderr=err,
        stdout=out,
    )
    return await communicate(*args, err=b"", proc=proc)


async def cmd_env(
    *args: object,
    env: dict[str, object] = {},
    err: ProcessInput = PIPE,
    log: bool = True,
    out: ProcessInput = None,
) -> bytes:
    return await _cmd_env(
        *(str(arg) for arg in args), env=env, err=err, log=log, out=out
    )


async def _cmd_flog(*args: str, out: str | None = None) -> bytes:
    if out is None:
        proc = await create_subprocess_exec(*args, stderr=DEVNULL, stdout=DEVNULL)
        return await communicate(*args, err=b"logs in /dev/null\n", proc=proc)
    Path(out).parent.mkdir(exist_ok=True, parents=True)
    with Path(out).open("ab") as ostream:
        proc = await create_subprocess_exec(*args, stderr=ostream, stdout=ostream)
        return await communicate(*args, err=f"logs in {out}\n".encode(), proc=proc)


async def cmd_flog(*args: object, out: str | None = None) -> bytes:
    return await _cmd_flog(*(str(arg) for arg in args), out=out)


async def communicate(
    *args: str, err: bytes, proc: Process, input: bytes | None = None
) -> bytes:
    cmd_stderr = cmd_stdout = None
    try:
        cmd_stdout, cmd_stderr = await (
            proc.communicate(input=input) if input else proc.communicate()
        )
    finally:
        cmd_stdout = cmd_stdout or b""
        cmd_stderr = cmd_stderr or b""
        returncode = proc.returncode
        if returncode is None:
            proc.terminate()
            returncode = await proc.wait()
            if proc.stderr is not None:
                cmd_stderr += await proc.stderr.read()
            if proc.stdout is not None:
                cmd_stdout += await proc.stdout.read()
        if returncode != 0:
            if not isinstance(exc_info()[1], CancelledError):
                raise ProcessError(
                    *args,
                    stdout=cmd_stdout,
                    stderr=err + cmd_stderr,
                    returncode=returncode or 1,
                )
    return cmd_stdout or b""


@contextmanager
def main() -> Iterator[None]:
    try:
        yield
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        get_logger().info("Exiting...")
