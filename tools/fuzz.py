from asyncio import run
from asyncio.subprocess import PIPE
from itertools import chain, repeat
from pathlib import Path
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd


async def fuzz(fuzzer: str, dictionary: str, *dirs: str) -> None:
    await cmd(
        f"./fuzz-{fuzzer}",
        f"-dict={dictionary}",
        "-max_len=16384",
        "-max_total_time=240",
        "-print_final_stats=1",
        "-reduce_inputs=1",
        "-shrink=1",
        "-use_value_profile=1",
        *filter(
            Path.is_dir,  # type: ignore
            chain.from_iterable(map(Path.rglob, map(Path, dirs), repeat("*"))),  # type: ignore
        ),
        err=PIPE,
        log=False,
        out=PIPE,
        timeout=300,
    )


if __name__ == "__main__":
    try:
        run(fuzz(*argv[1:]))
    except ProcessError:
        pass
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
