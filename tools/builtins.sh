#!/bin/bash

eval $1/builtins < stdlib/_builtins.py | \
clang-format -style=file > library/virtual_machine/builtins.cpp

clang-tidy -p="$1" \
-quiet -fix -fix-errors -format-style=file -config='' -header-filter=.\* \
library/virtual_machine/builtins.cpp
