from pathlib import Path
from sys import path

tools = str(Path(__file__).parent.absolute())

print(
    ":".join(
        map(lambda p: str(Path(p).absolute()), filter(lambda p: p != tools, path)),
    )
)
