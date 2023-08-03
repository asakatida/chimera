// Copyright (c) 2017 Asa Katida <github@holomaplefeline.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//! wrapper for Input*
//! provides global parse state tracking

#include "grammar/input.hpp"

#include <tao/pegtl/istream_input.hpp> // for istream_input

#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stack>

using namespace std::literals;

namespace chimera::library::grammar {
  [[nodiscard]] auto validateBasic(const std::string_view &line, char type)
      -> bool {
    return std::all_of(line.begin(), line.end(), [type](auto &&whitespace) {
      return whitespace == type;
    });
  }
  using NumbericLimits = std::numeric_limits<std::uint16_t>;
  constexpr static auto bufferSize = NumbericLimits::max();
  Input::Input(std::istream &input, const char *source)
      : InputBase{input, bufferSize, source} {}
  [[nodiscard]] auto Input::dedent() -> bool {
    using namespace std::literals;
    if (!is_dedent()) {
      return false;
    }
    indentStack.pop();
    if (indentStack.empty()) {
      indentType = '\0';
    }
    if (empty()) {
      return true;
    }
    if ((indentStack.empty() && column() > 1) ||
        (!indentStack.empty() && column() > indentStack.top())) {
      throw tao::pegtl::parse_error("bad dedent"s, *this);
    }
    return true;
  }
  [[nodiscard]] auto Input::indent() -> bool {
    const auto col = column();
    if ((indentStack.empty() || indentStack.top() < col) && validate()) {
      indentStack.push(col);
      return true;
    }
    return false;
  }
  [[nodiscard]] auto Input::is_dedent() -> bool {
    if (indentStack.empty()) {
      return false;
    }
    return empty() || column() < indentStack.top();
  }
  [[nodiscard]] auto Input::is_newline() const -> bool {
    const auto col = column();
    if (indentStack.empty()) {
      return 1 == col;
    }
    return indentStack.top() == col;
  }
  [[nodiscard]] auto Input::validate() -> bool {
    const auto *begin = current();
    auto col = column();
    if (col == 0) {
      return false;
    }
    std::advance(begin, -col);
    while (*begin == '\n' || *begin == '\r') {
      col -= 1;
      if (col == 0) {
        return false;
      }
      std::advance(begin, 1);
    }
    const std::string_view line(begin, col);
    if (indentType == '\0') {
      if (validateBasic(line, *begin)) {
        indentType = *begin;
        return true;
      }
      return false;
    }
    return validateBasic(line, indentType);
  }
} // namespace chimera::library::grammar
