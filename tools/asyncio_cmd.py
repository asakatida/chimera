from asyncio import create_subprocess_exec, wait_for
from asyncio.subprocess import DEVNULL, PIPE
from sys import stderr
from typing import Optional


class ProcessError(Exception):
    def __init__(self, cmd: tuple[str, ...], stderr: bytes, returncode: int) -> None:
        super().__init__(f"{cmd} failed with {returncode}")
        self.cmd = cmd
        self.stderr = stderr.decode().strip()
        self.returncode = returncode

    def exit(self) -> None:
        print(*self.cmd, file=stderr)
        print(self.stderr, file=stderr)
        exit(self.returncode)


async def cmd(
    *args: str, log: bool = True, stdout: Optional[int] = DEVNULL, timeout: int = 20
) -> bytes:
    if log:
        print("+", *args, file=stderr)
    proc = await create_subprocess_exec(*args, stderr=PIPE, stdout=stdout)
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
            raise ProcessError(args, cmd_stderr, returncode or 1)
    return cmd_stdout
