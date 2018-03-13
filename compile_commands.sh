. cflags.sh

echo [

for file in $SOURCE/{library,source,tools,unit_tests}{/**,}/*.cpp
  echo "  {"

  echo "    \"directory\": \"$SOURCE\","
  echo "    \"command\": \"$CXX $CPPFLAGS $CXXFLAGS -c $file\","
  echo "    \"file\": \"$file\""

  echo "  },"
end

echo ]
