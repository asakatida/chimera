from asyncio import run
from os import environ
from pathlib import Path
from sys import argv

from asyncio_cmd import cmd, cmd_env, main, splitlines
from structlog import get_logger


async def pip_install(file: object) -> None:
    for line in splitlines(await cmd_env("pip", "install", "-r", file)):
        if not line.startswith("Requirement already satisfied: "):
            get_logger().info(line)


async def action(script: str, *args: str) -> None:
    environ["PATH"] = ":".join(
        (
            "/opt/virtualenv/bin",
            "/home/github/.cargo/bin",
            "/usr/lib/ninja-build/bin",
            environ["PATH"],
        )
    )
    await pip_install("requirements.core.txt")
    await pip_install("requirements.txt")
    source = Path(__file__).parent.parent.resolve()
    environ["CCACHE_CONFIGPATH"] = str(source / ".github" / "ccache" / "ccache.conf")
    environ["CCACHE_DIR"] = str(source / ".ccache" / "local")
    environ["CCACHE_REMOTE_STORAGE"] = (
        f"file:{source / '.ccache' / 'remote'}|update-mtime=true"
    )
    await cmd("/bin/sh", "-e", "-c", script, *args, err=None, out=None)


if __name__ == "__main__":
    with main():
        run(action(*argv[1:]))
