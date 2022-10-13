from pathlib import Path
from sys import path

tools = Path(__file__).resolve().parent

paths: list[Path] = list(map(Path.resolve, map(Path, path)))  # type: ignore
paths.remove(tools)
print(*paths, sep=":")
