# Copyright (c) 2018 Adam Grandquist <grandquista@gmail.com>
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

"""
numbers.py.
"""

from pathlib import Path
from re import M
from re import compile as rc
from re import escape
from typing import (Callable, Dict, Iterable, Iterator, Match, Pattern, TextIO,
                    Tuple, cast)

OPERATIONS = {
    'add': 'operator+',
    'div': 'operator/',
    'floor_div': 'floor_div',
    'mod': 'operator%',
    'mult': 'operator*',
    'sub': 'operator-',
}

OPERATIONS_COMP = {
    'compare': 'operator==',
    'less': 'operator<',
}

OPERATIONS_BIT = {
    'and': 'operator&',
    'left_shift': 'operator<<',
    'or': 'operator|',
    'right_shift': 'operator>>',
    'xor': 'operator^',
}

OPERATIONS_UNARY = {
    'invert': 'operator~',
    'negative': 'operator-',
    'positive': 'operator+',
}

BASE = 'std::uint64_t'

TYPES = (
    'Base',
    'Natural',
    'Integer',
    'Rational',
)

PAIRINGS = tuple(
    (left, right)
    for left in (BASE,) + TYPES
    for right in (BASE,) + TYPES
    if left != BASE or right != BASE
)

NOT_BIT_TYPES = (
    'Rational',
)

TYPE_RE = '|'.join(map(escape, (BASE,) + TYPES))

OP_LINE_RE_START = r'(^(?P<indent> +)(\S.*)'
OP_LINE_RE_END = (
    r'\((const\s+)?(?P<left>'
    + TYPE_RE
    + r')(?s:.+?)(const\s+)?(?P<right>'
    + TYPE_RE
    + r')(.+?)$)')
OP_UNARY_LINE_RE_END = (
    r'\((const\s+)?(?P<left>'
    + TYPE_RE
    + r')(.+?)$)')

ROOT = Path(__file__).resolve().parent.parent / 'library' / 'object' / 'number'


def passing_type(typ: str) -> str:
    """
    Passing type.
    """
    if typ in (BASE, TYPES[0]):
        return f'{ typ } '
    return f'const { typ } &'


def op_header(
        match: Match[str],
        pairs: Iterable[Tuple[str, str]]) -> Iterator[str]:
    """
    Header operation.
    """
    for pair in pairs:
        left, right = pair
        if (
                match.group('left') == left
                and match.group('right') == right):
            return
        yield from iter((
            match.group('indent'),
            'Base ',
            match.group('op'),
            '(',
            passing_type(left),
            'left, ',
            passing_type(right),
            'right);\n'))


def op_source(
        match: Match[str],
        pairs: Iterable[Tuple[str, str]]) -> Iterator[str]:
    """
    Source operation.
    """
    for pair in pairs:
        left, right = pair
        if (
                match.group('left') == left
                and match.group('right') == right):
            return
        indent = match.group('indent')
        yield from iter((
            indent,
            'Base ',
            match.group('op'),
            '(',
            passing_type(left),
            '/*left*/, ',
            passing_type(right),
            '/*right*/) {\n',
            indent,
            '  Expects(false);\n',
            indent,
            '}\n'))


def op_bit_header(
        match: Match[str],
        pairs: Iterable[Tuple[str, str]]) -> Iterator[str]:
    """
    Header operation.
    """
    return op_header(match, pairs)


def op_bit_source(
        match: Match[str],
        pairs: Iterable[Tuple[str, str]]) -> Iterator[str]:
    """
    Source operation.
    """
    return op_source(match, pairs)


def op_comp_header(
        match: Match[str],
        pairs: Iterable[Tuple[str, str]]) -> Iterator[str]:
    """
    Header operation.
    """
    return op_header(match, pairs)


def op_comp_source(
        match: Match[str],
        pairs: Iterable[Tuple[str, str]]) -> Iterator[str]:
    """
    Source operation.
    """
    return op_source(match, pairs)


def op_unary_header(
        match: Match[str],
        types: Iterable[str]) -> Iterator[str]:
    """
    Header operation.
    """
    for typ in types:
        if match.group('left') == typ:
            return
        yield from iter((
            match.group('indent'),
            'Base ',
            match.group('op'),
            '(',
            passing_type(typ),
            'value);\n'))


def op_unary_source(
        match: Match[str],
        types: Iterable[str]) -> Iterator[str]:
    """
    Source operation.
    """
    for typ in types:
        if match.group('left') == typ:
            return
        indent = match.group('indent')
        yield from iter((
            indent,
            'Base ',
            match.group('op'),
            '(',
            passing_type(typ),
            '/*value*/) {\n',
            indent,
            '  Expects(false);\n',
            indent,
            '}\n'))


Callback = Callable[
    [Match[str], Iterable[Tuple[str, str]]], Iterator[str]]
UnaryCallback = Callable[
    [Match[str], Iterable[str]], Iterator[str]]

OPERATIONS_CALLBACK = {
    'add': (op_header, op_source),
    'and': (op_bit_header, op_bit_source),
    'compare': (op_comp_header, op_comp_source),
    'div': (op_header, op_source),
    'floor_div': (op_header, op_source),
    'left_shift': (op_bit_header, op_bit_source),
    'less': (op_comp_header, op_comp_source),
    'mod': (op_header, op_source),
    'mult': (op_header, op_source),
    'or': (op_bit_header, op_bit_source),
    'right_shift': (op_bit_header, op_bit_source),
    'sub': (op_header, op_source),
    'xor': (op_bit_header, op_bit_source),
}


def op_line_re(op: str) -> Pattern[str]:
    """
    Operation line regex.
    """
    return rc(
        OP_LINE_RE_START
        + r'(?P<op>'
        + escape(op)
        + ')'
        + OP_LINE_RE_END,
        M)


def op_unary_line_re(op: str) -> Pattern[str]:
    """
    Operation line regex.
    """
    return rc(
        OP_LINE_RE_START
        + r'(?P<op>'
        + escape(op)
        + ')'
        + OP_UNARY_LINE_RE_END,
        M)


def sources(name: str) -> Tuple[Path, Path]:
    """
    Header and source for operation name.
    """
    source = ROOT / name
    return (source.with_suffix('.hpp'), source.with_suffix('.cpp'))


def process_lines(
        src: str,
        matches: Iterable[Match[str]],
        callback: Callback) -> Iterator[str]:
    """
    Process lines.
    """
    pairs = iter(PAIRINGS)
    end = 0
    for match in matches:
        yield src[end:match.start()]
        end = match.end()
        yield from callback(match, pairs)
        yield match.group()
    yield from callback(match, pairs)
    yield src[end:]


def process_unary_lines(
        src: str,
        matches: Iterable[Match[str]],
        callback: UnaryCallback) -> Iterator[str]:
    """
    Process lines.
    """
    types = iter(TYPES)
    end = 0
    for match in matches:
        yield src[end:match.start()]
        end = match.end()
        yield from callback(match, types)
        yield match.group()
    yield from callback(match, types)
    yield src[end:]


def process_header(op: str, header: Path, callback: Callback) -> None:
    """
    Process header.
    """
    with cast(TextIO, header.open()) as istream:
        src = istream.read()
    matches = iter(op_line_re(op).finditer(src))
    with cast(TextIO, header.open('w')) as ostream:
        ostream.writelines(process_lines(src, matches, callback))


def process_source(op: str, source: Path, callback: Callback) -> None:
    """
    Process header.
    """
    with cast(TextIO, source.open()) as istream:
        src = istream.read()
    matches = iter(op_line_re(op).finditer(src))
    with cast(TextIO, source.open('w')) as ostream:
        ostream.writelines(process_lines(src, matches, callback))


def process_unary_header(
        op: str,
        header: Path,
        callback: UnaryCallback) -> None:
    """
    Process header.
    """
    with cast(TextIO, header.open()) as istream:
        src = istream.read()
    matches = iter(op_unary_line_re(op).finditer(src))
    with cast(TextIO, header.open('w')) as ostream:
        ostream.writelines(process_unary_lines(src, matches, callback))


def process_unary_source(
        op: str,
        source: Path,
        callback: UnaryCallback) -> None:
    """
    Process header.
    """
    with cast(TextIO, source.open()) as istream:
        src = istream.read()
    matches = iter(op_unary_line_re(op).finditer(src))
    with cast(TextIO, source.open('w')) as ostream:
        ostream.writelines(process_unary_lines(src, matches, callback))


def process_ops(ops: Dict[str, str]) -> None:
    """
    Process operations.
    """
    for name, op in ops.items():
        header, source = sources(name)
        header_callback, source_callback = OPERATIONS_CALLBACK[name]
        process_header(op, header, header_callback)
        process_source(op, source, source_callback)


def process_unary_ops(ops: Dict[str, str]) -> None:
    """
    Process operations.
    """
    for name, op in ops.items():
        header, source = sources(name)
        process_unary_header(op, header, op_unary_header)
        process_unary_source(op, source, op_unary_source)


def main() -> None:
    """
    Main entry.
    """
    process_ops(OPERATIONS)
    process_ops(OPERATIONS_COMP)
    process_ops(OPERATIONS_BIT)
    process_unary_ops(OPERATIONS_UNARY)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print()
