from pathlib import Path
from sys import path

tools = Path(__file__).parent.resolve()

paths: list[Path] = list(map(Path.resolve, map(Path, path)))  # type: ignore
paths.remove(tools)
print(*paths, sep=":")
