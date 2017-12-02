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

"""generate_utf8_id_insane.py."""

from itertools import product

id_start = set(filter(lambda i: chr(i).isidentifier(), range(0x10FFFF)))
id_continue = set(
    filter(
        lambda i: all(
            map(lambda j: ''.join((chr(j), chr(i))).isidentifier(), id_start)),
        set(range(0x10FFFF)) - id_start))
print(
    list(
        next(
            filter(
                str.isidentifier,
                map(
                    ''.join,
                    product(
                        map(chr, id_start),
                        map(
                            chr,
                            set(
                                range(0x10FFFF)).difference(
                                    id_start,
                                    id_continue))))))
        .encode()))
