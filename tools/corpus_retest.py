"""corpus_retest.py."""

from asyncio import run
from pathlib import Path
from re import escape, finditer
from sys import argv

from asyncio_as_completed import as_completed
from asyncio_cmd import ProcessError, cmd_flog, main
from cmake_codecov import cmake_codecov
from corpus_utils import corpus_merge, corpus_trim, fuzz_star, regression
from structlog import get_logger

SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CRASHES = FUZZ / "crashes"


def fuzz_output_paths(prefix: bytes, output: bytes) -> frozenset[bytes]:
    return frozenset(
        m["path"] for m in finditer(escape(prefix) + rb"\s+(?P<path>\S+)", output)
    )


async def regression_log_one(fuzzer: Path, *chunk: Path) -> Exception | None:
    log_file = Path("/tmp") / f"{fuzzer.name}-{chunk[0].parent.name}.log"
    log_file.write_bytes(b"")
    try:
        await cmd_flog(fuzzer, *chunk, out=str(log_file))
    except ProcessError as err:
        return err
    finally:
        log_output = log_file.read_bytes()
        for file in (
            Path(path.decode())
            for path in fuzz_output_paths(b"Running:", log_output)
            - fuzz_output_paths(b"Executed", log_output)
        ):
            try:
                file.rename(CRASHES / (file.parent.name + file.name))
            except FileNotFoundError:
                pass
        log_file.unlink(missing_ok=True)
    (chunk[0].parent / ".done").touch()
    return None


async def regression_log() -> list[Exception]:
    return [
        exc
        for exc in await as_completed(
            regression_log_one(fuzz, *args)
            for args in (
                frozenset(
                    path
                    for path in corpus.glob("*")
                    if path.is_file() and path.name != ".done"
                )
                for corpus in CORPUS.glob("*")
                if not (corpus / ".done").exists()
            )
            for fuzz in fuzz_star()
            if args
        )
        if exc is not None
    ]


async def corpus_retest() -> None:
    CRASHES.mkdir(exist_ok=True, parents=True)
    while await regression_log():
        await get_logger().ainfo(
            "Regression failed, retrying with"
            f" {len([path for path in CORPUS.rglob('*') if path.is_file()])} cases"
        )
    for done in CORPUS.rglob(".done"):
        done.unlink()
    await corpus_merge(disable_bars=None)


async def corpus_retest_main(build: str) -> None:
    await cmake_codecov("fuzzers", "fuzz-chimera")
    await corpus_retest()
    await regression(build)
    corpus_trim(disable_bars=None)


if __name__ == "__main__":
    with main():
        run(corpus_retest_main(*argv[1:]))
