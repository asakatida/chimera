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
      struct Space
          : tao::pegtl::star<tao::pegtl::sor<
                tao::pegtl::seq<tao::pegtl::one<'\\'>, tao::pegtl::eol>,
                tao::pegtl::seq<
                    tao::pegtl::one<'#'>,
                    tao::pegtl::star<tao::pegtl::not_at<tao::pegtl::eolf>,
                                     tao::pegtl::utf8::any>>,
                std::conditional_t<
                    Implicit, Utf8Space,
                    tao::pegtl::minus<Utf8Space,
                                      tao::pegtl::one<'\r', '\n'>>>>> {};
      template <bool Discard>
      struct BlankLines;
      template <>
      struct BlankLines<true>
          : tao::pegtl::seq<
                tao::pegtl::plus<Space<false>, tao::pegtl::eol,
                                 tao::pegtl::discard>,
                tao::pegtl::sor<tao::pegtl::plus<tao::pegtl::one<' '>>,
                                tao::pegtl::star<tao::pegtl::one<'\t'>>>,
                tao::pegtl::must<
                    tao::pegtl::not_at<tao::pegtl::one<' ', '\t'>>>,
                tao::pegtl::discard> {};
      template <>
      struct BlankLines<false>
          : tao::pegtl::seq<
                tao::pegtl::plus<Space<false>, tao::pegtl::eol>,
                tao::pegtl::sor<tao::pegtl::plus<tao::pegtl::one<' '>>,
                                tao::pegtl::star<tao::pegtl::one<'\t'>>>,
                tao::pegtl::must<
                    tao::pegtl::not_at<tao::pegtl::one<' ', '\t'>>>> {};
      struct IndentCheck : tao::pegtl::not_at<tao::pegtl::one<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.indent();
        }
      };
      struct INDENT : tao::pegtl::seq<BlankLines<true>, IndentCheck> {};
      struct DedentCheck : tao::pegtl::not_at<tao::pegtl::one<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.is_dedent();
        }
      };
      struct DedentConsume : tao::pegtl::not_at<tao::pegtl::one<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.dedent();
        }
      };
      struct DEDENT
          : tao::pegtl::sor<
                tao::pegtl::seq<tao::pegtl::eof, DedentConsume>,
                tao::pegtl::seq<
                    tao::pegtl::at<BlankLines<false>, DedentCheck>,
                    tao::pegtl::opt<BlankLines<false>, DedentConsume>>> {};
      struct NextIndentCheck : tao::pegtl::not_at<tao::pegtl::one<' ', '\t'>> {
        template <typename Input>
        static bool match(Input &&in) {
          return in.is_newline();
        }
      };
      struct NEWLINE : tao::pegtl::seq<BlankLines<true>, NextIndentCheck> {};
      template <bool Implicit, typename... Rules>
      using Token = tao::pegtl::seq<Rules..., Space<Implicit>>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
