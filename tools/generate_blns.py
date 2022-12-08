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

"""generate_blns.py."""

from base64 import standard_b64decode, standard_b64encode
from itertools import count
from json import loads
from pathlib import Path

print('#include "runtime.hpp"')
print("")
print("#include <sstream>")
print("")
print("#include <catch2/catch_test_macros.hpp>")
print("")
print("#include <tao/pegtl.hpp>")
print("")
print('#include "base64.hpp"')
set(
    map(
        lambda n, s: print(
            """
TEST_CASE("BLNS test """
            f"{n}"
            """") {
  """
            f"""{
    'auto str = Testing::base64_decode("{}")'.format(
        standard_b64encode(s).decode())
    if 0 in s or max(s) > 0x7F else (
        'std::string str(R"EOF({})EOF", {})'
        if 10 in s or 13 in s or 0x22 in s or 0x5C in s else
        'std::string str("{}", {})'
    ).format(s.decode(), len(s))
  };"""
            """
  chimera::library::grammar::BufferInput<std::istringstream> input(
      "<testing>", chimera::BUFFER_SIZE, str);
  std::variant<chimera::asdl::Module, chimera::asdl::Interactive> top;
  REQUIRE_THROWS_AS(
      (tao::pegtl::parse<chimera::library::grammar::FileInput>(
          input, top)),
      tao::pegtl::parse_error &);
}"""
        ),
        count(),
        filter(
            None,
            map(
                standard_b64decode,
                loads(
                    Path(
                        "external/big-list-of-naughty-strings/blns.base64.json"
                    ).read_text()
                ),
            ),
        ),
    )
)
