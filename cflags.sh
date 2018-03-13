cd (dirname (status --current-filename))

set -x SOURCE (pwd)

set -x CPPFLAGS \
  $CPPFLAGS \
  -I $SOURCE/include \
  -I $SOURCE/library \
  -I $SOURCE/unit_tests \
  -isystem $SOURCE/external/GSL/include \
  -isystem $SOURCE/external/metal/include \
  -isystem $SOURCE/external/PEGTL/include \
  -isystem $SOURCE/external/Catch/single_include

set -x CXXFLAGS \
  $CXXFLAGS \
  -Werror \
  "-DCHIMERA_PATH=/usr/local/Cellar/python3/3.6.3/Frameworks/Python.framework/Versions/3.6/lib/python3.6/:$SOURCE/stdlib/" \
  "-Wno-c++98-compat-pedantic" \
  -Wno-date-time \
  -Wno-padded \
  "-ftemplate-depth=1536"
