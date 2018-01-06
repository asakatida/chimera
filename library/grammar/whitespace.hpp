// Copyright (c) 2017 Adam Grandquist <grandquista@gmail.com>
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

//! parse definitions for token separators.
//! depends on chimera::library::grammar::Input specialization

#pragma once

#include <type_traits>

#include "grammar/rules.hpp"
#include "grammar/utf8_space.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <typename Option>
      using Space = Star<Sor<
          Seq<One<'\\'>, Eol>, Seq<One<'#'>, Star<NotAt<Eolf>, Any>>,
          std::conditional_t<Option::template get<Option::IMPLICIT>, Utf8Space,
                             Minus<Utf8Space, One<'\r', '\n'>>>>>;
      template <typename Option>
      struct BlankLines
          : Seq<Plus<Space<Option>, Eol,
                     std::conditional_t<Option::template get<Option::DISCARD>,
                                        Discard, Success>>,
                Sor<Plus<One<' '>>, Star<One<'\t'>>>,
                Must<NotAt<One<' ', '\t'>>>,
                std::conditional_t<Option::template get<Option::DISCARD>,
                                   Discard, Success>> {};
      struct IndentCheck : NotAt<One<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.indent();
        }
      };
      template <typename Option>
      using INDENT = Seq<BlankLines<Option>, IndentCheck>;
      struct DedentCheck : NotAt<One<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.is_dedent();
        }
      };
      struct DedentConsume : NotAt<One<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.dedent();
        }
      };
      template <typename Option>
      using DEDENT = Sor<Seq<Eof, DedentConsume>,
                         Seq<At<BlankLines<Option>, DedentCheck>,
                             Opt<BlankLines<Option>, DedentConsume, Discard>>>;
      struct NextIndentCheck : NotAt<One<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.is_newline();
        }
      };
      template <typename Option>
      using NEWLINE = Seq<BlankLines<Option>, NextIndentCheck, Discard>;
      template <typename Option, typename... Rules>
      using Token = Seq<Rules..., Space<Option>>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
