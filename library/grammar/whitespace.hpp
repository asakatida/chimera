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
  using Utf8NonLineBreak = minus<Utf8Space, one<'\n', '\r'>>;
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
