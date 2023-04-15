from asyncio import run
from itertools import chain, repeat
from pathlib import Path
from sys import argv, stderr

from asyncio_cmd import ProcessError, cmd


async def fuzz(fuzzer: str, dictionary: str, *dirs: str) -> None:
    await cmd(
        f"./fuzz-{fuzzer}",
        "-dict",
        dictionary,
        "-max_len",
        16384,
        "-max_total_time",
        120,
        "-print_final_stats",
        1,
        "-reduce_inputs",
        1,
        "-shrink",
        1,
        "-use_value_profile",
        1,
        *filter(
            Path.is_dir,  # type: ignore
            chain.from_iterable(map(Path.rglob, map(Path, dirs), repeat("*"))),  # type: ignore
        ),
        err=None,
        out=None,
        timeout=120,
    )


if __name__ == "__main__":
    try:
        run(fuzz(*argv[1:]))
    except ProcessError:
        pass
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
