from asyncio import run
from asyncio.subprocess import PIPE
from os import environ
from pathlib import Path
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd_env, cmd_no_timeout


async def pip_install(file: object) -> None:
    for line in (
        await cmd_env("pip", "install", "-r", file, out=PIPE, err=None, timeout=120)
    ).splitlines():
        if not line.startswith(b"Requirement already satisfied: "):
            print(line.decode(), file=stderr)


async def main() -> None:
    environ["PATH"] = ":".join(
        ("/opt/virtualenv/bin", "/home/github/.cargo/bin", environ["PATH"])
    )
    await pip_install("requirements.core.txt")
    await pip_install("requirements.txt")
    source = Path(__file__).parent.parent.resolve()
    environ["CCACHE_CONFIGPATH"] = str(source / ".github" / "ccache" / "ccache.conf")
    environ["CCACHE_DIR"] = str(source / ".ccache")
    environ["CXXFLAGS"] = environ.get("CXXFLAGS", "").translate(
        dict(zip(b"\n\r", "  "))
    )
    await cmd_no_timeout("/bin/bash", "-eo", "pipefail", "-c", *argv[1:])


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
