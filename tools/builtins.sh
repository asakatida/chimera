#!/usr/bin/env bash

SOURCE="$(dirname "$0")"

"${SOURCE}/builtins" < stdlib/_builtins.py | clang-format -style=file > library/virtual_machine/builtins.cpp

clang-tidy -p="${SOURCE}" -quiet -fix -fix-errors -format-style=file -config='' -header-filter='.*' library/virtual_machine/builtins.cpp
