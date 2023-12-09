from pathlib import Path
from sys import path

tools = Path(__file__).parent.resolve()

paths = [Path(p).resolve() for p in path]
paths.remove(tools)
print(*paths, sep=":")
