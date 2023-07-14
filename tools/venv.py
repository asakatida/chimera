from pathlib import Path
from subprocess import run
from sys import executable

from chimera_utils import rmdir


def pip(*args: str, env: dict[str, str]) -> None:
    run(["env/bin/pip", *args], env=env)


def main() -> None:
    rmdir(Path("env"))
    env: dict[str, str] = {}
    run([executable, "-m", "venv", "env"], env=env)
    pip("install", "-r", "requirements.core.txt", env=env)
    pip("install", "-r", "requirements.txt", env=env)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nExiting...")
