from asyncio import run
from pathlib import Path
from sys import stderr

from asyncio_cmd import ProcessError, cmd


async def venv(python: str) -> None:
    source = Path(__file__).resolve().parent.parent
    env = source / "env"
    await cmd(python, "-m", "venv", "--upgrade", "--upgrade-deps", env)
    pip = env / "bin" / "pip"
    await cmd(pip, "install", "--upgrade", "pip", "setuptools", "wheel")
    await cmd(pip, "install", "-r", "requirements.txt", timeout=120)


async def main() -> None:
    await venv("python3")


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
