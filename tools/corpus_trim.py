"""corpus_trim.py"""

from asyncio_cmd import main
from corpus_utils import corpus_trim

if __name__ == "__main__":
    with main():
        corpus_trim(disable_bars=None)
