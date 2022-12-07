from asyncio import run
from os import environ
from pathlib import Path
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd


async def main() -> None:
    environ["PATH"] = ":".join(
        ("/opt/virtualenv/bin", "/usr/local/bin", environ["PATH"])
    )
    source = Path(__file__).parent.parent.resolve()
    environ["CCACHE_CONFIGPATH"] = str(source / ".github" / "ccache" / "ccache.conf")
    environ["CCACHE_DIR"] = str(source / ".ccache")
    environ["CXXFLAGS"] = environ.get("CXXFLAGS", "").translate(
        {ord("\n"): " ", ord("\r"): " "}
    )
    await cmd(
        "/bin/bash",
        "-eo",
        "pipefail",
        "-c",
        *argv[1:],
        stdout=None,
        timeout=3600,
    )


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
