from asyncio import run
from pathlib import Path
from sys import argv, stderr

from asyncio_as_completed import a_list, as_completed
from asyncio_cmd import ProcessError, chunks, cmd_flog


async def regression(fuzzer: str, corpus: str) -> None:
    await a_list(
        as_completed(
            map(
                lambda args: cmd_flog(
                    f"./fuzz-{fuzzer}", "-detect_leaks=0", *args, timeout=120
                ),
                chunks(Path(corpus).iterdir(), 4096),
            ),
            limit=2,
        )
    )


if __name__ == "__main__":
    try:
        run(regression(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
