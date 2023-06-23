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

#include "grammar/flags.hpp"
#include "grammar/rules.hpp"
#include "grammar/utf8_space.hpp"

#include <type_traits>

namespace chimera::library::grammar {
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
  template <char... Chars>
  using String = tao::pegtl::ascii::string<Chars...>;
#else
  template <char... Chars>
  using String = tao::pegtl::utf8::string<Chars...>;
#endif
  using Eol = sor<String<'\r', '\n'>, one<'\r', '\n'>>;
  using Eolf = sor<eof, Eol>;
  template <flags::Flag Option>
  using Space = star<
      sor<seq<one<'\\'>, Eol>, seq<one<'#'>, star<not_at<Eolf>, any>>,
          std::conditional_t<flags::get<Option, flags::IMPLICIT>, Utf8Space,
                             minus<Utf8Space, one<'\r', '\n'>>>>,
      std::conditional_t<flags::get<Option, flags::DISCARD>, discard, success>>;
  using BlankLines = seq<plus<Space<0>, Eol>, star<Utf8NonLineBreak>>;
  struct Indent : not_at<Utf8NonLineBreak> {
    template <typename Input, typename... Args>
    static auto match(Input &&input, Args &&.../*args*/) -> bool {
      return input.indent();
    }
  };
  struct INDENT : seq<BlankLines, Indent, discard> {};
  struct Dedent : not_at<Utf8NonLineBreak> {
    template <typename Input, typename... Args>
    static auto match(Input &&input, Args &&.../*args*/) -> bool {
      return input.dedent();
    }
  };
  struct DEDENT : seq<sor<eof, BlankLines>, Dedent, discard> {};
  struct NextIndentCheck : not_at<Utf8NonLineBreak> {
    template <typename Input, typename... Args>
    static auto match(Input &&input, Args &&.../*args*/) -> bool {
      return input.is_newline();
    }
  };
  struct NEWLINE : seq<BlankLines, NextIndentCheck, discard> {};
  namespace token {
    template <auto Asdl>
    struct ConstantToken {
      template <typename Top, typename... Args>
      static void apply0(Top &&top, Args &&.../*args*/) {
        top.push(Asdl);
      }
    };
    template <flags::Flag Option, typename Rule>
    using Token = seq<Rule, Space<Option>>;
  } // namespace token
} // namespace chimera::library::grammar
