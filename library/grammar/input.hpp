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

#pragma once

#include <cstdint>
#include <iterator>
#include <stack>

// NOLINTBEGIN(misc-no-recursion)

#include <tao/pegtl.hpp>

namespace chimera::library::grammar {
  template <typename Base>
  struct Input : Base {
    using Base::Base;
    using Base::column;
    using Base::current;
    using Base::empty;
    [[nodiscard]] auto indent() -> bool {
      const auto col = column();
      if ((indentStack.empty() || indentStack.top() < col) && validate()) {
        indentStack.push(col);
        return true;
      }
      return false;
    }
    [[nodiscard]] auto is_dedent() -> bool {
      if (indentStack.empty()) {
        return false;
      }
      return empty() || column() < indentStack.top();
    }
    [[nodiscard]] auto dedent() -> bool {
      using namespace std::literals;
      if (indentStack.empty()) {
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
    [[nodiscard]] auto is_newline() const -> bool {
      const auto col = column();
      if (indentStack.empty()) {
        return 1 == col;
      }
      return indentStack.top() == col;
    }
    [[nodiscard]] auto validate() -> bool {
      auto begin = current();
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
    template <typename Line>
    [[nodiscard]] auto validateBasic(Line &&line, char type) -> bool {
      return std::all_of(line.begin(), line.end(), [type](auto &&whitespace) {
        return whitespace == type;
      });
    }

  private:
    char indentType = '\0';
    std::stack<std::uintmax_t> indentStack{};
  };
} // namespace chimera::library::grammar

// NOLINTEND(misc-no-recursion)
