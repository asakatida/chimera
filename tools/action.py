from asyncio import run
from os import environ
from pathlib import Path
from sys import argv

from asyncio_cmd import cmd_env, cmd_no_timeout, main, splitlines
from structlog import get_logger


async def pip_install(file: object) -> None:
    for line in splitlines(await cmd_env("pip", "install", "-r", file, timeout=120)):
        if not line.startswith("Requirement already satisfied: "):
            get_logger().info(line)


async def action(cmd: str, *args: str) -> None:
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
    await cmd_no_timeout("/bin/bash", "-eo", "pipefail", "-c", cmd, *args)


if __name__ == "__main__":
    with main():
        run(action(*argv[1:]))
