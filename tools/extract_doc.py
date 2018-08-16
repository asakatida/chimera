# Copyright (c) 2017 Asa Katida <github@holomaplefeline.net>
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

"""extract_doc.py."""

from html.parser import HTMLParser
from pathlib import Path
from re import sub
from sys import argv
from typing import List, Tuple


class Extract(HTMLParser):
    """Extract."""

    def __init__(self) -> None:
        """__init__."""
        super().__init__()
        self.c_type = ''
        self.c_code = False
        self.code = 0
        self.div = 0
        self.dl = 0
        self.data = ''
        self.enable = False
        self.name = ''
        self.unclosed_type = False
        self.actions = {
            'data': self.data_act,
            'function': self.function_act,
            'macro': self.macro_act,
            'member': self.member_act,
            'type': self.type_act,
            'var': self.var_act}

    def handle_starttag(self, tag: str, attrs: List[Tuple[str, str]]) -> None:
        """handle_starttag."""
        if tag == 'body':
            self.enable = True
        if not self.enable:
            return
        if tag == 'dl':
            if not self.dl and self.unclosed_type:
                print('};')
                self.unclosed_type = False
            self.dl += 1
            self.c_type = dict(attrs).get('class', '')
        if not self.c_type:
            return
        if self.dl and tag == 'code':
            self.code += 1
        elif self.code and tag == 'div':
            if dict(attrs).get('class', None) == 'highlight-c':
                self.c_code = True
            if self.c_code:
                self.div += 1

    def handle_endtag(self, tag: str) -> None:
        """handle_endtag."""
        if tag == 'body':
            self.enable = False
        if not self.enable:
            return
        if tag == 'dl':
            self.dl -= 1
            if not self.dl:
                self.data = self.data.strip()
                action = self.actions.get(self.c_type, None)
                if action:
                    action()
                else:
                    self.print_comment()
                self.c_type = ''
                self.data = ''
                self.name = ''
        elif self.dl and tag == 'code':
            self.code -= 1
        elif self.code and tag == 'div':
            if self.c_code:
                self.div -= 1
            if not self.div:
                self.c_code = False

    def handle_data(self, data: str) -> None:
        """handle_data."""
        if self.code and not self.name:
            self.name = data.strip()
        elif self.name and self.c_code:
            self.data += data.strip() or ' '
        elif self.dl:
            print('//', data)

    def data_act(self) -> None:
        """data_act."""
        print('extern int', self.name, self.data, end=';\n')

    def print_comment(self) -> None:
        """print_comment."""
        print('// ', self.c_type, ':', self.name, self.data.replace('\n', ' '))

    def function_act(self) -> None:
        """function_act."""
        print('extern', self.name, self.data, end=';\n')

    def macro_act(self) -> None:
        """macro_act."""
        if '\n' in self.data:
            self.data = self.data.replace('\n', ' ')
        print('#define', self.name, self.data)

    def member_act(self) -> None:
        """member_act."""
        print(sub(r'\w+\.', '', self.name), self.data, ';')

    def type_act(self) -> None:
        """type_act."""
        if 'struct ' in self.name:
            self.name = self.name.replace('struct ', '')
        if 'Py' in self.name:
            print('typedef struct', self.name, self.name, ';')
        print('struct', self.name, '{')
        self.unclosed_type = True

    def var_act(self) -> None:
        """var_act."""
        print('extern', self.name, self.data, ';')


def mapper(p: Path) -> None:
    """mapper."""
    print('/* start', p.name, '*/')
    extract = Extract()
    extract.feed(p.read_text())
    extract.close()
    print('/* end', p.name, '*/')


def main() -> None:
    """main."""
    header = '''#pragma once

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#ifndef _PyObject_EXTRA_INIT
#define _PyObject_EXTRA_INIT
#endif
#pragma clang diagnostic pop

typedef int64_t PY_INT64_T;
typedef int64_t Py_ssize_t;
typedef int64_t Py_hash_t;
typedef wchar_t Py_UNICODE;

typedef void *PyGILState_STATE;

typedef void (*PyOS_sighandler_t)(int);
'''
    print(header)
    root = Path(argv[1])
    files = sorted(root.iterdir())
    start = [
        root / 'structures.html',
        root / 'typeobj.html',
        root / 'type.html']
    set(map(files.remove, start))
    set(map(mapper, start))
    set(map(mapper, files))
    footer = '''
#ifdef __cplusplus
}
#endif'''
    print(footer)


if __name__ == '__main__':
    main()
