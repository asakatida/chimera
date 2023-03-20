from asyncio import run
from pathlib import Path
from sys import argv, stderr

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, chunks, cmd_flog


async def regression(fuzzer: str, corpus: str) -> None:
    await as_completed(
        map(
            lambda args: cmd_flog(f"./fuzz-{fuzzer}", *args, timeout=120),
            chunks(filter(Path.is_file, Path(corpus).rglob("*")), 4096),
        ),
        limit=2,
        cancel=True,
    )


if __name__ == "__main__":
    try:
        run(regression(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
