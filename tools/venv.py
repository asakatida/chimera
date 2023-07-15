from pathlib import Path
from subprocess import run
from sys import argv, executable

from chimera_utils import rmdir


def pip(venv: str, *args: str, env: dict[str, str]) -> None:
    run([f"{venv}/bin/pip", *args], env=env)


def main(venv: str = "env") -> None:
    rmdir(Path(venv))
    env: dict[str, str] = {}
    run([executable, "-m", "venv", venv], env=env)
    pip(venv, "install", "-r", "requirements.core.txt", env=env)
    pip(venv, "install", "-r", "requirements.txt", env=env)


if __name__ == "__main__":
    try:
        main(*argv[1:])
    except KeyboardInterrupt:
        print("\nExiting...")
