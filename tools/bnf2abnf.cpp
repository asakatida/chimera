//! helper to generate abnf from bnf

#include <tao/pegtl/ascii.hpp>         // for one, identifier
#include <tao/pegtl/istream_input.hpp> // for istream_input
#include <tao/pegtl/nothing.hpp>       // for nothing
#include <tao/pegtl/parse.hpp>         // for parse
#include <tao/pegtl/rules.hpp>         // for seq, discard (ptr only), plus

#include <algorithm> // for replace
#include <iostream>  // for operator<<, cout, cin

struct expression;

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
struct Spaces : tao::pegtl::plus<tao::pegtl::one<'\t', ' '>> {};

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
