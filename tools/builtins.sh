output=library/virtual_machine/builtins.cpp

cat stdlib/_builtins.py | \
$1/builtins | \
clang-format -style=file > $output

clang-tidy -p="$1" \
-quiet -fix -fix-errors -format-style=file -config='' -header-filter=.* \
$output
