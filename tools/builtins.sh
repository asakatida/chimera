cat stdlib/_builtins.py | \
eval $1/builtins | \
clang-format -style=file > library/virtual_machine/builtins.cpp

clang-tidy -p="$1" \
-quiet -fix -fix-errors -format-style=file -config='' -header-filter=.\* \
library/virtual_machine/builtins.cpp
