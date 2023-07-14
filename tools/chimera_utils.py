from pathlib import Path


def rmdir(path: Path) -> None:
    if path.is_dir():
        for child in path.iterdir():
            rmdir(child)
        path.rmdir()
    else:
        path.unlink(missing_ok=True)
