"""corpus_merge.py"""

from asyncio import run
from pathlib import Path
from sys import argv

from asyncio_cmd import main
from cmake_codecov import cmake_codecov
from corpus_freeze import corpus_freeze
from corpus_utils import corpus_gather, corpus_merge, corpus_trim

SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"


async def corpus_merge_main(base_reference: str = "HEAD") -> None:
    await cmake_codecov("fuzzers", "fuzz-chimera")
    await corpus_gather(
        "unit_tests/fuzz/corpus", base_reference=base_reference, disable_bars=None
    )
    await corpus_merge(disable_bars=None)
    Path("unit_tests/fuzz/cases.json").write_text("{}")
    await corpus_freeze(
        "unit_tests/fuzz/cases.json", base_reference=base_reference, disable_bars=None
    )
    corpus_trim(disable_bars=None)


if __name__ == "__main__":
    with main():
        run(corpus_merge_main(*argv[1:]))
