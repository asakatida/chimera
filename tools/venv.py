from pathlib import Path
from subprocess import run
from sys import executable


def rmdir(path: Path) -> None:
    if path.is_dir():
        for child in path.iterdir():
            rmdir(child)
        path.rmdir()
    else:
        path.unlink(missing_ok=True)


def pip(*args: str, env: dict[str, str]) -> None:
    run(
        ["env/bin/pip", *args],
        env=env,
        timeout=120,
    )


def main() -> None:
    rmdir(Path("env"))
    env: dict[str, str] = {}
    run([executable, "-m", "venv", "env"], env=env, timeout=10)
    pip("install", "-r", "requirements.core.txt", env=env)
    pip("install", "-r", "requirements.txt", env=env)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nExiting...")