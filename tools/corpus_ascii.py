"""corpus_ascii.py"""

from itertools import groupby
from pathlib import Path
from string import printable
from typing import Iterable

from asyncio_cmd import main
from corpus_utils import bucket, gather_paths
from structlog import get_logger


def is_ascii(data: bytes) -> bool:
    try:
        return all(c in printable for c in data.decode())
    except UnicodeDecodeError:
        return False


def _is_ascii(path: Path) -> bool:
    return is_ascii(path.read_bytes())


def corpus_ascii() -> Iterable[Path]:
    return (path for path in gather_paths() if _is_ascii(path))


if __name__ == "__main__":
    with main():
        paths = sorted(corpus_ascii())
        for file in paths:
            get_logger().info(file)
        for name, contents in groupby(paths, lambda file: bucket(file).name):
            total = len([elem for elem in contents])
            get_logger().info(f"{name}: {total} / {total * 100 // len(paths) / 100}")
