output=library/virtual_machine/importlib.cpp

$1/builtins \
clang-format -style=file > $output

clang-tidy -p="$1" \
-quiet -fix -fix-errors -format-style=file -config='' -header-filter=.* \
$output
