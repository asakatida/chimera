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

#include "grammar/flags.hpp"
#include "grammar/rules.hpp"
#include "grammar/utf8_space.hpp"

namespace chimera::library::grammar {
  template <char... Chars>
  using String = tao::pegtl::utf8::string<Chars...>;
  using Eol = sor<String<'\r', '\n'>, one<'\r', '\n'>>;
  using Eolf = sor<eof, Eol>;
  template <flags::Flag Option>
  using Space = star<
      sor<seq<one<'\\'>, Eol>, seq<one<'#'>, star<not_at<Eolf>, any>>,
          std::conditional_t<flags::get<Option, flags::IMPLICIT>, Utf8Space,
                             minus<Utf8Space, one<'\r', '\n'>>>>,
      std::conditional_t<flags::get<Option, flags::DISCARD>, discard, success>>;
  using BlankLines = seq<plus<Space<0>, Eol>, star<Utf8NonLineBreak>>;
  struct IndentCheck : not_at<Utf8NonLineBreak> {
    template <typename Input, typename... Args>
    static auto match(Input &&in, Args &&...args) -> bool {
      return in.indent();
    }
  };
  struct INDENT : seq<BlankLines, IndentCheck, discard> {};
  struct DedentCheck : not_at<Utf8NonLineBreak> {
    template <typename Input, typename... Args>
    static auto match(Input &&in, Args &&...args) -> bool {
      return in.is_dedent();
    }
  };
  struct DedentConsume : not_at<Utf8NonLineBreak> {
    template <typename Input, typename... Args>
    static auto match(Input &&in, Args &&...args) -> bool {
      return in.dedent();
    }
  };
  struct DEDENT : sor<seq<eof, DedentConsume>,
                      seq<at<BlankLines, DedentCheck>,
                          opt<BlankLines, DedentConsume, discard>>> {};
  struct NextIndentCheck : not_at<Utf8NonLineBreak> {
    template <typename Input, typename... Args>
    static auto match(Input &&in, Args &&...args) -> bool {
      return in.is_newline();
    }
  };
  struct NEWLINE : seq<BlankLines, NextIndentCheck, discard> {};
  namespace token {
    template <auto Asdl>
    struct ConstantToken {
      template <typename Top, typename... Args>
      static void apply0(Top &&top, Args &&...args) {
        top.push(Asdl);
      }
    };
    template <flags::Flag Option, typename Rule>
    using Token = seq<Rule, Space<Option>>;
  } // namespace token
} // namespace chimera::library::grammar
