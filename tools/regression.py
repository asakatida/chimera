from asyncio import run
from sys import argv

from asyncio_cmd import main
from corpus_utils import regression

if __name__ == "__main__":
    with main():
        run(regression(*argv[1:]))
