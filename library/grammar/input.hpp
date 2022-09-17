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

#include <tao/pegtl.hpp>

namespace chimera::library::grammar {
  template <typename Base>
  struct Input : Base {
    using Base::Base;
    using Base::column;
    using Base::current;
    using Base::empty;
    auto indent() -> bool {
      const auto i = column();
      if ((indentStack.empty() || indentStack.top() < i) && validate()) {
        indentStack.push(i);
        return true;
      }
      return false;
    }
    [[nodiscard]] auto is_dedent() const -> bool {
      if (indentStack.empty()) {
        return false;
      }
      return column() < indentStack.top();
    }
    auto dedent() -> bool {
      using namespace std::literals;
      if (indentStack.empty()) {
        return false;
      }
      indentStack.pop();
      if (empty()) {
        return true;
      }
      if (column() > indentStack.top()) {
        throw tao::pegtl::parse_error("bad dedent"s, *this);
      }
      return true;
    }
    [[nodiscard]] auto is_newline() const -> bool {
      const auto i = column();
      if (indentStack.empty()) {
        return 0 == i;
      }
      return indentStack.top() == i;
    }
    auto validate() -> bool {
      auto begin = current();
      const auto i = column();
      std::advance(begin, -i);
      std::string_view line(begin, i);
      if (indentType == 0) {
        indentType = *begin;
        if (auto result = validateBasic(line); !result) {
          indentType = 0;
          return result;
        }
        return true;
      }
      return validateBasic(line);
    }
    template <typename Line>
    auto validateBasic(Line &&line) -> bool {
      return std::all_of(line.begin(), line.end(),
                         [this](auto &&c) { return c == indentType; });
    }

  private:
    std::uint32_t indentType = 0;
    std::stack<std::uintmax_t> indentStack;
  };
} // namespace chimera::library::grammar
