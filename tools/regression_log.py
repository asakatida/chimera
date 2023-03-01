from asyncio import run
from pathlib import Path
from sys import argv, stderr

from asyncio_as_completed import a_list, as_completed
from asyncio_cmd import ProcessError, chunks, cmd_flog


async def regression(fuzzer: str, corpus: str) -> None:
    log_file = f"/tmp/{fuzzer}.log"
    Path(log_file).write_bytes(b"")
    try:
        await a_list(
            as_completed(
                map(
                    lambda args: cmd_flog(
                        f"./fuzz-{fuzzer}",
                        "-detect_leaks=0",
                        *args,
                        out=log_file,
                        timeout=600,
                    ),
                    chunks(Path(corpus).iterdir(), 4096),
                ),
                limit=3,
            )
        )
    finally:
        log_output = Path(log_file).read_bytes()
        last_line = b""
        for line in log_output.splitlines():
            if line.startswith(b"terminate called "):
                Path(last_line.split(maxsplit=1)[1].decode()).unlink(missing_ok=True)
            last_line = line


if __name__ == "__main__":
    try:
        run(regression(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
