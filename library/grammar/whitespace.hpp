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

//! parse definitions for token separators.
//! depends on chimera::library::grammar::Input specialization

#pragma once

#include <type_traits>

#include <tao/pegtl.hpp>

#include "grammar/utf8_space.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <bool Implicit>
      using Space =
          Star<Sor<Seq<One<'\\'>, Eol>, Seq<One<'#'>, Star<NotAt<Eolf>, Any>>,
                   std::conditional_t<Implicit, Utf8Space,
                                      Minus<Utf8Space, One<'\r', '\n'>>>>>;
      template <bool Discard>
      struct BlankLines;
      template <>
      struct BlankLines<true> : Seq<Plus<Space<false>, Eol, Discard>,
                                    Sor<Plus<One<' '>>, Star<One<'\t'>>>,
                                    Must<NotAt<One<' ', '\t'>>>, Discard> {};
      template <>
      struct BlankLines<false>
          : Seq<Plus<Space<false>, Eol>, Sor<Plus<One<' '>>, Star<One<'\t'>>>,
                Must<NotAt<One<' ', '\t'>>>> {};
      struct IndentCheck : NotAt<One<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.indent();
        }
      };
      using INDENT = Seq<BlankLines<true>, IndentCheck>;
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
      using DEDENT = Sor<Seq<Eof, DedentConsume>,
                         Seq<At<BlankLines<false>, DedentCheck>,
                             Opt<BlankLines<false>, DedentConsume, Discard>>>;
      struct NextIndentCheck : NotAt<One<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.is_newline();
        }
      };
      using NEWLINE = Seq<BlankLines<false>, NextIndentCheck, Discard>;
      template <bool Implicit, typename... Rules>
      using Token = Seq<Rules..., Space<Implicit>>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
