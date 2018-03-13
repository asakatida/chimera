. cflags.sh

set -l COMPILE $CXX $CPPFLAGS $CXXFLAGS
set -l FORMAT clang-format -style=file -i
set -l TIDY clang-tidy \
  -p="$SOURCE" \
  -fix -fix-errors -format-style=file -config='' -header-filter="\.\*"

for file in $SOURCE/{library,source,tools,unit_tests}{/**,}/*.cpp
  eval $COMPILE -o "$file.o" -c "$file"
  if test $status -gt 0
    exit
  end
  eval $FORMAT "$file"
  if test $status -gt 0
    exit
  end
  eval $TIDY "$file"
  if test $status -gt 0
    exit
  end
end

for file in $SOURCE/{include,library,source,tools,unit_tests}{/**,}/*.h{pp,}
  eval $FORMAT "$file"
  if test $status -gt 0
    exit
  end
end
