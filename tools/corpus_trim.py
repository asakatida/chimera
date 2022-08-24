# Copyright (c) 2022 Asa Katida <github@holomaplefeline.net>
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

"""corpus_trim.py"""

from hashlib import sha256
from pathlib import Path

LENGTH = 6


def sha(path: Path) -> str:
    return sha256(path.read_bytes()).hexdigest()


corpus = (Path(__file__).parent.parent / "unit_tests" / "fuzz" / "corpus").absolute()

for file in corpus.glob("*"):
    name = file.name[:LENGTH].ljust(LENGTH, "0")
    if (corpus / name).exists() and sha(file) != sha(corpus / name):
        print("Collision found, update corpus_trim.py `LENGTH`:", LENGTH)
        exit(1)
    file.rename(corpus / name)
