from pathlib import Path
from sys import path

tools = Path(__file__).parent.resolve()

paths = [Path(p).resolve() for p in path]
paths.remove(tools)
paths.append(tools.parent / "stdlib")

if __name__ == "__main__":
    print(*paths, sep=":")
else:
    cppflags = f'-DCHIMERA_PATH="{":".join(str(path) for path in paths)}"'
