from asyncio import run
from itertools import chain, repeat
from pathlib import Path
from sys import argv

from asyncio_cmd import ProcessError, cmd, main


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
        log=False,
        timeout=1200,
    )


if __name__ == "__main__":
    with main():
        try:
            run(fuzz(*argv[1:]))
        except ProcessError:
            pass
