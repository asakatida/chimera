"""corpus_gather.py"""

from asyncio import run
from sys import argv

from asyncio_cmd import main
from corpus_utils import corpus_gather


async def corpus_gather_main(ref: str) -> None:
    if ref == "refs/heads/stable":
        await corpus_gather("unit_tests/fuzz/corpus", disable_bars=False)


if __name__ == "__main__":
    with main():
        run(corpus_gather_main(*argv[1:]))
