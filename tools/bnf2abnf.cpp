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

//! helper to generate abnf from bnf

#include <algorithm>
#include <iostream>
#include <string_view>

#include <tao/pegtl.hpp>

#include "grammar/utf8_space.hpp"

template <typename Rule>
struct BnfAction : tao::pegtl::nothing<Rule> {};

template <char Constant>
struct BnfPrint {
  template <typename... Args>
  static void apply0(Args &&.../*args*/) {
    std::cout << Constant;
  }
};

struct CommentBody : tao::pegtl::until<tao::pegtl::eolf> {};

template <>
struct BnfAction<CommentBody> {
  template <typename Input, typename... Args>
  static void apply(const Input &input, Args &&.../*args*/) {
    std::cout << ';' << input.string();
  }
};

struct Comment : tao::pegtl::seq<tao::pegtl::one<'#'>, CommentBody> {};
struct Spaces : tao::pegtl::plus<chimera::library::grammar::Utf8NonLineBreak> {
};

template <>
struct BnfAction<Spaces> : BnfPrint<' '> {};

struct OptWhitespace
    : tao::pegtl::star<
          tao::pegtl::sor<Spaces, tao::pegtl::one<'\n', '\r'>, Comment>,
          tao::pegtl::discard> {};
struct RuleName : tao::pegtl::identifier {};

template <>
struct BnfAction<RuleName> {
  template <typename Input, typename... Args>
  static void apply(const Input &input, Args &&.../*args*/) {
    auto str = input.string();
    std::replace(str.begin(), str.end(), '_', '-');
    std::cout << str;
  }
};

struct TermIdentifier
    : tao::pegtl::seq<tao::pegtl::identifier,
                      tao::pegtl::not_at<OptWhitespace, tao::pegtl::one<':'>>> {
};

template <>
struct BnfAction<TermIdentifier> {
  template <typename Input, typename... Args>
  static void apply(const Input &input, Args &&.../*args*/) {
    auto str = input.string();
    std::replace(str.begin(), str.end(), '_', '-');
    std::cout << str;
  }
};

struct Literal : tao::pegtl::seq<tao::pegtl::one<'\''>,
                                 tao::pegtl::until<tao::pegtl::one<'\''>>> {};

template <>
struct BnfAction<Literal> {
  template <typename Input, typename... Args>
  static void apply(const Input &input, Args &&.../*args*/) {
    std::cout << '"' << input.string().substr(1, input.string().size() - 2)
              << '"';
  }
};

struct expression;

struct GroupStart : tao::pegtl::one<'('> {};

template <>
struct BnfAction<GroupStart> : BnfPrint<'('> {};

struct GroupEnd : tao::pegtl::one<')'> {};

template <>
struct BnfAction<GroupEnd> : BnfPrint<')'> {};

struct Group : tao::pegtl::seq<GroupStart, OptWhitespace, expression, GroupEnd,
                               OptWhitespace, tao::pegtl::discard> {};

struct OptionStart : tao::pegtl::one<'['> {};

template <>
struct BnfAction<OptionStart> : BnfPrint<'['> {};

struct OptionEnd : tao::pegtl::one<']'> {};

template <>
struct BnfAction<OptionEnd> : BnfPrint<']'> {};

struct Option : tao::pegtl::seq<OptionStart, OptWhitespace, expression,
                                OptionEnd, OptWhitespace, tao::pegtl::discard> {
};
struct Term : tao::pegtl::sor<Literal, TermIdentifier, Group, Option> {};

struct BnfStar : tao::pegtl::at<Term, tao::pegtl::one<'*'>> {};

template <>
struct BnfAction<BnfStar> : BnfPrint<'*'> {};

struct BnfPlus : tao::pegtl::at<Term, tao::pegtl::one<'+'>> {};

template <>
struct BnfAction<BnfPlus> {
  template <typename... Args>
  static void apply0(Args &&.../*args*/) {
    std::cout << "1*";
  }
};

struct Modifier
    : tao::pegtl::sor<
          tao::pegtl::seq<BnfStar, Term, tao::pegtl::one<'*'>, OptWhitespace>,
          tao::pegtl::seq<BnfPlus, Term, tao::pegtl::one<'+'>, OptWhitespace>,
          Term> {};
struct BnfList
    : tao::pegtl::plus<Modifier, OptWhitespace, tao::pegtl::discard> {};

struct Sep : tao::pegtl::one<'|'> {};

template <>
struct BnfAction<Sep> : BnfPrint<'/'> {};

struct expression
    : tao::pegtl::list_must<BnfList, tao::pegtl::seq<Sep, OptWhitespace>> {};

struct Assign : tao::pegtl::one<':'> {};

template <>
struct BnfAction<Assign> : BnfPrint<'='> {};

struct Rule : tao::pegtl::seq<RuleName, Assign, OptWhitespace, expression,
                              tao::pegtl::discard> {};

template <>
struct BnfAction<Rule> : BnfPrint<'\n'> {};

struct Syntax
    : tao::pegtl::seq<OptWhitespace, tao::pegtl::plus<Rule>, tao::pegtl::eof> {
};

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main() -> int {
  tao::pegtl::istream_input<> istream(std::cin, 1024, "<input>");
  return tao::pegtl::parse<Syntax, BnfAction>(istream) ? 0 : 1;
}
