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
#include <stack>

#include <tao/pegtl.hpp>

namespace chimera::library::grammar {
  template <typename Base>
  struct Input : Base {
    template <typename... Args>
    explicit Input(Args &&...args) : Base(std::forward<Args>(args)...) {
      indentStack.emplace();
    }
    auto indent() -> bool {
      std::uintmax_t i = Base::column();
      if (indentStack.top() < i) {
        indentStack.push(i);
        return true;
      }
      return false;
    }
    [[nodiscard]] auto is_dedent() const -> bool {
      return Base::column() < indentStack.top();
    }
    auto dedent() -> bool {
      using namespace std::literals;
      indentStack.pop();
      if (Base::empty()) {
        return true;
      }
      std::uintmax_t i = Base::column();
      if (i > indentStack.top()) {
        throw tao::pegtl::parse_error("bad dedent"s, *this);
      }
      return i == indentStack.top();
    }
    [[nodiscard]] auto is_newline() const -> bool {
      return Base::column() == indentStack.top();
    }

  private:
    std::stack<std::uintmax_t> indentStack{};
  };
} // namespace chimera::library::grammar
