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

#include <exception>                   // for throw_with_nested
#include <tao/pegtl/istream_input.hpp> // for istream_input

#include <cstdint>
#include <iosfwd>
#include <stack>
#include <variant>

namespace chimera::library::grammar {
  using InputBase = tao::pegtl::istream_input<>;
  struct Input : InputBase {
    Input(std::istream &input, const char *source);
    [[nodiscard]] auto dedent() -> bool;
    [[nodiscard]] auto indent() -> bool;
    [[nodiscard]] auto is_newline() const -> bool;

  private:
    [[nodiscard]] auto is_dedent() -> bool;
    [[nodiscard]] auto validate() -> bool;
    char indentType = '\0';
    std::stack<std::uintmax_t> indentStack{};
  };
} // namespace chimera::library::grammar
