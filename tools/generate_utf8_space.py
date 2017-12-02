# Copyright (c) 2017 Adam Grandquist <grandquista@gmail.com>
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

"""generate_utf8_space.py."""

from itertools import chain, count, groupby, starmap
from typing import Iterable, Tuple


def _a(key: int, group: Iterable[Tuple[int, int]]) -> Tuple[int, ...]:
    return tuple(e[0] for e in group)


print('#pragma once')
print('')
print('#include <tao/pegtl.hpp>')
print('')
print('namespace chimera {')
print('namespace library {')
print('namespace grammar {')
print('struct Utf8Space:')
print('tao::pegtl::utf8::ranges<')
print(
    *chain.from_iterable(
        map(
            lambda t: (min(t), max(t)),
            starmap(
                _a,
                groupby(
                    zip(
                        filter(lambda i: chr(i).isspace(), range(0x10FFFF)),
                        count()),
                    lambda t: t[0] - t[1])))),
    sep=', ')
print('> {};')
print('}  // namespace grammar')
print('}  // namespace library')
print('}  // namespace chimera')
