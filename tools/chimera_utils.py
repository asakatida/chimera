from os import environ
from pathlib import Path

IN_CI = environ.get("CI", "") == "true"


def rmdir(path: Path) -> None:
    if path.is_dir():
        for child in path.iterdir():
            rmdir(child)
        path.rmdir()
    else:
        path.unlink(missing_ok=True)
