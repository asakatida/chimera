# Copyright (c) 2023 Asa Katida <github@holomaplefeline.net>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""corpus_merge.py"""

from asyncio import run
from pathlib import Path

from asyncio_cmd import main
from corpus_freeze import corpus_freeze
from corpus_gather import corpus_gather
from corpus_utils import corpus_merge, corpus_trim

SOURCE = Path(__file__).parent.parent.resolve()
FUZZ = SOURCE / "unit_tests" / "fuzz"
CORPUS = FUZZ / "corpus"
CORPUS_ORIGINAL = FUZZ / "corpus_original"


async def corpus_merge_main() -> None:
    await corpus_gather("unit_tests/fuzz/corpus", disable_bars=None)
    corpus_trim(disable_bars=None)
    await corpus_merge(disable_bars=None)
    await corpus_freeze("unit_tests/fuzz/cases.json", disable_bars=None)


if __name__ == "__main__":
    with main():
        run(corpus_merge_main())
