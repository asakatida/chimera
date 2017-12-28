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

//! tao::pegtl::* wrappers to control stack specialization

#pragma once

#include <metal.hpp>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/rep_one_min_max.hpp>

#include "grammar/rules/action.hpp"
#include "grammar/rules/captures.hpp"
#include "grammar/rules/control.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace rules {
        template <typename Type>
        struct Optional {};

        template <typename... Types>
        struct Tuple {};

        template <typename... Types>
        struct Variant {};

        template <typename Type>
        struct Vector {};

        using Any = tao::pegtl::utf8::any;
        template <char32_t... Chars>
        using NotOne = tao::pegtl::utf8::not_one<Chars...>;
        template <char32_t... Chars>
        using NotRange = tao::pegtl::utf8::not_range<Chars...>;
        template <char32_t... Chars>
        using One = tao::pegtl::utf8::one<Chars...>;
        template <char32_t... Chars>
        using Range = tao::pegtl::utf8::range<Chars...>;
        template <char32_t... Chars>
        using Ranges = tao::pegtl::utf8::ranges<Chars...>;
        template <char32_t... Chars>
        using String = tao::pegtl::utf8::string<Chars...>;

        using Digit = Range<'0', '9'>;
        using Ellipsis = tao::pegtl::seq<One<'.'>, One<'.'>, One<'.'>>;
        using Eof = tao::pegtl::eof;
        using Eol = tao::pegtl::sor<String<'\r', '\n'>, One<'\r'>, One<'\n'>>;
        using Eolf = tao::pegtl::sor<Eof, Eol>;
        using Nul = One<0>;
        using Seven = Range<0, 0b1111111>;
        using Xdigit = Ranges<'0', '9', 'a', 'f', 'A', 'F'>;

        template <template <typename...> typename A, typename... Rules>
        using Action = tao::pegtl::action<A, Rules...>;
        template <typename... Rules>
        using At = tao::pegtl::at<Rules...>;
        template <template <typename...> typename C, typename... Rules>
        using Control = tao::pegtl::control<C, Rules...>;
        using Discard = tao::pegtl::discard;
        using Failure = tao::pegtl::failure;
        template <typename... Rules>
        using IfMust = tao::pegtl::if_must<Rules...>;
        template <typename Test, typename Rule, typename Else>
        using IfThenElse = tao::pegtl::if_then_else<Test, Rule, Else>;
        template <typename... Rules>
        using List = tao::pegtl::list<Rules...>;
        template <typename... Rules>
        using ListMust = tao::pegtl::list_must<Rules...>;
        template <typename... Rules>
        using ListTail = tao::pegtl::list_tail<Rules...>;
        template <typename Rule, typename Test>
        using Minus = tao::pegtl::minus<Rule, Test>;
        template <typename... Rules>
        using Must = tao::pegtl::must<Rules...>;
        template <typename... Rules>
        using NotAt = tao::pegtl::not_at<Rules...>;
        template <typename... Rules>
        using Opt = tao::pegtl::opt<Rules...>;
        template <typename... Rules>
        using Plus = tao::pegtl::plus<Rules...>;
        template <unsigned Repeat, typename... Rules>
        using Rep = tao::pegtl::rep<Repeat, Rules...>;
        template <unsigned Repeat, typename... Rules>
        using RepOpt = tao::pegtl::rep_opt<Repeat, Rules...>;
        template <typename... Rules>
        using Seq = tao::pegtl::seq<Rules...>;
        template <typename... Rules>
        using Sor = tao::pegtl::sor<Rules...>;
        template <typename... Rules>
        using Star = tao::pegtl::star<Rules...>;
        using Success = tao::pegtl::success;
        template <typename Test, typename... Rules>
        using Until = tao::pegtl::until<Test, Rules...>;

        template <typename Rule>
        struct ASTType {
          using Type = Tuple<>;
        };
        template <typename... Rules>
        struct ASTType<Opt<Rules...>> {
          using Type = Tuple<>;
        };
        template <unsigned Repeat, typename... Rules>
        struct ASTType<Rep<Repeat, Rules...>> {
          using Type = Tuple<>;
        };
        template <typename... Rules>
        struct ASTType<Seq<Rules...>> {
          using Type = Tuple<>;
        };
        template <typename... Rules>
        struct ASTType<Sor<Rules...>> {
          using Type = Tuple<>;
        };
      } // namespace rules
    }   // namespace grammar
  }     // namespace library
} // namespace chimera
