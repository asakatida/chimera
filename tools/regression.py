from asyncio import run
from sys import argv

from asyncio_cmd import main
from corpus_utils import regression


async def regression_main(build: str, fuzzer: str = "") -> None:
    await regression(build, fuzzer)


if __name__ == "__main__":
    with main():
        run(regression_main(*argv[1:]))
