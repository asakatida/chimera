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

//! parse definitions for string tokens.

#pragma once

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <string>
#include <string_view>

#include <gsl/gsl>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/unescape.hpp>

#include "asdl/asdl.hpp"
#include "grammar/exprfwd.hpp"
#include "grammar/flags.hpp"
#include "grammar/oper.hpp"
#include "grammar/rules.hpp"
#include "grammar/whitespace.hpp"
#include "object/object.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

namespace chimera::library::grammar {
  namespace token {
    using namespace std::literals;
    struct StringHolder : rules::VariantCapture<object::Object> {
      std::string string;
      template <typename String>
      void apply(String &&in) {
        string.append(std::forward<String>(in));
      }
    };
    struct LiteralChar : plus<not_one<'\0', '{', '}'>> {};
    template <>
    struct Action<LiteralChar> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        top.apply(in.string());
      }
    };
    template <flags::Flag Option>
    using FExpression =
        sor<list_tail<sor<ConditionalExpression<Option>, StarExpr<Option>>,
                      Comma<Option>>,
            YieldExpr<Option>>;
    struct Conversion : one<'a', 'r', 's'> {};
    template <>
    struct Action<Conversion> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        asdl::FormattedValue formattedValue{
            top.template pop<asdl::ExprImpl>(), asdl::FormattedValue::STR, {}};
        switch (in.peek_char()) {
          case 'a':
            formattedValue.conversion = asdl::FormattedValue::ASCII;
            break;
          case 'r':
            formattedValue.conversion = asdl::FormattedValue::REPR;
            break;
          case 's':
            formattedValue.conversion = asdl::FormattedValue::STR;
            break;
          default:
            break;
        }
        top.push(std::move(formattedValue));
      }
    };
    template <flags::Flag Option>
    struct FormatSpec;
    template <flags::Flag Option>
    using ReplacementField =
        if_must<LBrt<Option>, FExpression<Option>, opt<one<'!'>, Conversion>,
                opt<one<':'>, FormatSpec<Option>>, RBrt<Option>>;
    template <flags::Flag Option>
    struct FormatSpec
        : star<sor<LiteralChar, one<0>, ReplacementField<Option>>> {};
    template <flags::Flag Option>
    struct Action<FormatSpec<Option>> {
      template <typename Top>
      static void apply0(Top &&top) {
        auto formatSpec = top.template pop<asdl::ExprImpl>();
        auto expr = top.template pop<asdl::ExprImpl>();
        if (std::holds_alternative<asdl::FormattedValue>(*expr.value)) {
          std::get<asdl::FormattedValue>(*expr.value).format_spec =
              std::move(formatSpec);
          top.push(std::move(expr));
        } else {
          top.push(asdl::FormattedValue{std::move(expr),
                                        asdl::FormattedValue::STR,
                                        std::move(formatSpec)});
        }
      }
    };
    struct LeftFLiteral : String<'{', '{'> {};
    template <>
    struct Action<LeftFLiteral> {
      template <typename Top>
      static void apply0(Top &&top) {
        top.apply("{"sv);
      }
    };
    struct RightFLiteral : String<'}', '}'> {};
    template <>
    struct Action<RightFLiteral> {
      template <typename Top>
      static void apply0(Top &&top) {
        top.apply("}"sv);
      }
    };
    struct FLiteral : plus<sor<LiteralChar, LeftFLiteral, RightFLiteral>> {
      using Transform = StringHolder;
    };
    template <>
    struct Action<FLiteral> {
      template <typename Top>
      static void apply0(Top &&top) {
        top.push(object::Object(object::String(top.string), {}));
      }
    };
    template <flags::Flag Option>
    using FString = must<star<sor<FLiteral, ReplacementField<Option>>>, eof>;
    template <typename Chars>
    struct SingleChars : plus<Chars> {};
    template <typename Chars>
    struct Action<SingleChars<Chars>> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        top.apply(in.string());
      }
    };
    template <unsigned Len>
    struct Hexseq : rep<Len, ranges<'0', '9', 'a', 'f', 'A', 'F'>> {};
    template <unsigned Len>
    struct Action<Hexseq<Len>> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        std::string string;
        if (tao::pegtl::unescape::utf8_append_utf32(
                string, tao::pegtl::unescape::unhex_string<std::uint32_t>(
                            in.begin(), in.end()))) {
          top.apply(std::move(string));
        }
      }
    };
    template <char Open, unsigned Len>
    using UTF = seq<one<Open>, Hexseq<Len>>;
    struct Octseq : seq<range<'0', '7'>, rep_opt<2, range<'0', '7'>>> {};
    template <>
    struct Action<Octseq> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        std::string string;
        if (tao::pegtl::unescape::utf8_append_utf32(
                string, std::accumulate(in.begin(), in.end(), std::uint32_t(0),
                                        [](const auto init, const auto c) {
                                          return (init << 2U) |
                                                 static_cast<std::uint32_t>(
                                                     c - '0');
                                        }))) {
          top.apply(std::move(string));
        }
      }
    };
    struct EscapeControl : one<'a', 'b', 'f', 'n', 'r', 't', 'v'> {};
    template <>
    struct Action<EscapeControl> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        switch (in.peek_char()) {
          case 'a':
            top.apply("\a"sv);
            break;
          case 'b':
            top.apply("\b"sv);
            break;
          case 'f':
            top.apply("\f"sv);
            break;
          case 'n':
            top.apply("\n"sv);
            break;
          case 'r':
            top.apply("\r"sv);
            break;
          case 't':
            top.apply("\t"sv);
            break;
          case 'v':
            top.apply("\v"sv);
            break;
          default:
            Expects(false);
        }
      }
    };
    template <typename Chars>
    struct EscapeIgnore : seq<Chars> {};
    template <typename Chars>
    struct Action<EscapeIgnore<Chars>> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        top.apply(R"(\)"sv);
        top.apply(in.string());
      }
    };
    using Escape = one<'\\'>;
    using XEscapeseq = UTF<'x', 2>;
    template <typename Chars, typename... Escapes>
    using Escapeseq = sor<Escapes..., XEscapeseq, Octseq, Eol, EscapeControl,
                          EscapeIgnore<Chars>>;
    template <typename Chars, typename... Escapes>
    using Item = seq<if_then_else<Escape, Escapeseq<Chars, Escapes...>,
                                  SingleChars<minus<Chars, Escape>>>,
                     discard>;
    template <typename Chars>
    using RawItem = if_then_else<Escape, Chars, Chars>;
    template <typename Triple, typename Chars, typename... Escapes>
    using Long =
        if_must<Triple,
                until<Triple, Item<seq<not_at<Triple>, Chars>, Escapes...>>>;
    template <typename Triple, typename Chars>
    struct LongRaw
        : if_must<Triple, until<Triple, RawItem<seq<not_at<Triple>, Chars>>>> {
    };
    template <typename Triple, typename Chars>
    struct Action<LongRaw<Triple, Chars>> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        std::string_view view(in.begin(), in.size());
        view.remove_prefix(3);
        view.remove_suffix(3);
        top.apply(view);
      }
    };
    template <typename Quote, typename Chars, typename... Escapes>
    using Short = if_must<
        Quote,
        until<Quote, Item<minus<seq<not_at<Quote>, Chars>, Eol>, Escapes...>>>;
    template <typename Quote, typename Chars>
    struct ShortRaw
        : if_must<
              Quote,
              until<Quote, RawItem<minus<seq<not_at<Quote>, Chars>, Eol>>>> {};
    template <typename Quote, typename Chars>
    struct Action<ShortRaw<Quote, Chars>> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        std::string_view view(in.begin(), in.size());
        view.remove_prefix(1);
        view.remove_suffix(1);
        top.apply(view);
      }
    };
    using TripleSingle = rep<3, one<'\''>>;
    using TripleDouble = rep<3, one<'"'>>;
    using Single = one<'\''>;
    using Double = one<'"'>;
    template <typename Chars>
    using Raw = sor<LongRaw<TripleDouble, Chars>, LongRaw<TripleSingle, Chars>,
                    ShortRaw<Double, Chars>, ShortRaw<Single, Chars>>;
    template <typename Chars, typename... Escapes>
    using Escaped =
        sor<Long<TripleDouble, Chars, Escapes...>,
            Long<TripleSingle, Chars, Escapes...>,
            Short<Double, Chars, Escapes...>, Short<Single, Chars, Escapes...>>;
    using UTF16Escape = UTF<'u', 4>;
    using UTF32Escape = UTF<'U', 8>;
    struct UName : star<not_one<'}'>> {};
    template <>
    struct Action<UName> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        top.apply(in.string());
      }
    };
    using UNameEscape = if_must<String<'N', '{'>, UName, one<'}'>>;
    template <typename Prefix, typename RawPrefix, typename Chars,
              typename... Escapes>
    using StringImpl = sor<seq<RawPrefix, Raw<Chars>>,
                           seq<Prefix, Escaped<Chars, Escapes...>>>;
    using BytesPrefix = one<'b', 'B'>;
    using BytesRawPrefix = sor<seq<one<'r', 'R'>, one<'b', 'B'>>,
                               seq<one<'b', 'B'>, one<'r', 'R'>>>;
    template <flags::Flag Option>
    struct Bytes : plus<Token<Option, StringImpl<BytesPrefix, BytesRawPrefix,
                                                 range<0, 0b1111111>>>> {
      struct Transform : rules::VariantCapture<object::Object> {
        object::Bytes bytes;
        template <typename String>
        void apply(String &&in) {
          for (const auto &byte : in) {
            bytes.emplace_back(static_cast<std::uint8_t>(byte));
          }
        }
      };
    };
    template <flags::Flag Option>
    struct Action<Bytes<Option>> {
      template <typename Top>
      static void apply0(Top &&top) {
        top.push(object::Object(std::move(top.bytes), {}));
      }
    };
    using StrPrefix = opt<one<'u', 'U'>>;
    using StrRawPrefix = one<'r', 'R'>;
    template <flags::Flag Option>
    struct DocString
        : plus<Token<Option,
                     StringImpl<StrPrefix, StrRawPrefix, any, UTF16Escape,
                                UTF32Escape, UNameEscape>>> {
      using Transform = StringHolder;
    };
    template <flags::Flag Option>
    struct Action<DocString<Option>> {
      template <typename Top>
      static void apply0(Top &&top) {
        top.push(object::Object(object::String(top.string), {}));
      }
    };
    using JoinedStrPrefix = one<'f', 'F'>;
    using JoinedStrRawPrefix = sor<seq<one<'r', 'R'>, one<'f', 'F'>>,
                                   seq<one<'f', 'F'>, one<'r', 'R'>>>;
    struct PartialString
        : plus<StringImpl<StrPrefix, StrRawPrefix, any, UTF16Escape,
                          UTF32Escape, UNameEscape>> {
      struct Transform {
        std::string string;
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(std::move(string));
        }
        template <typename String>
        void apply(String &&in) {
          string.append(std::forward<String>(in));
        }
      };
    };
    template <flags::Flag Option>
    struct FormattedString
        : seq<StringImpl<JoinedStrPrefix, JoinedStrRawPrefix, any, UTF16Escape,
                         UTF32Escape, UNameEscape>> {
      struct Transform : rules::Stack<asdl::ExprImpl> {
        std::string string;
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 0) {
            asdl::JoinedStr joinedStr;
            joinedStr.values.reserve(s);
            transform<asdl::ExprImpl>(std::back_inserter(joinedStr.values));
            outer.push(std::move(joinedStr));
          }
        }
        template <typename String>
        void apply(String &&in) {
          string.append(std::forward<String>(in));
        }
      };
    };
    template <flags::Flag Option>
    struct Action<FormattedString<Option>> {
      template <typename Input, typename Top>
      static void apply(const Input &in, Top &&top) {
        auto result = tao::pegtl::parse_nested<
            FString<flags::list<flags::DISCARD, flags::IMPLICIT>>, Action,
            Normal>(in,
                    tao::pegtl::memory_input<>(top.string.c_str(),
                                               top.string.size(), "<f_string>"),
                    std::forward<Top>(top));
        Ensures(result);
      }
    };
    template <flags::Flag Option>
    struct JoinedStrOne
        : plus<Token<Option, sor<PartialString, FormattedString<Option>>>> {
      using Transform = rules::VariantCapture<std::string, asdl::JoinedStr>;
    };
    template <flags::Flag Option>
    struct Action<JoinedStrOne<Option>> {
      using State = std::variant<std::string, asdl::JoinedStr>;
      struct Visitor {
        auto operator()(std::string &&value, std::string &&element) {
          value.append(element);
          return State{std::move(value)};
        }
        auto operator()(std::string &&value, asdl::JoinedStr &&joinedStr) {
          joinedStr.values.emplace(joinedStr.values.begin(),
                                   object::Object(object::String(value), {}));
          return State{std::move(joinedStr)};
        }
        auto operator()(asdl::JoinedStr &&value, std::string &&element) {
          value.values.emplace_back(
              object::Object(object::String(element), {}));
          return State{std::move(value)};
        }
        auto operator()(asdl::JoinedStr &&value, asdl::JoinedStr &&joinedStr) {
          std::move(joinedStr.values.begin(), joinedStr.values.end(),
                    std::back_inserter(value.values));
          return State{std::move(value)};
        }
      };
      template <typename Top>
      static void apply0(Top &&top) {
        State value;
        for (auto &&element : top.vector()) {
          value = std::visit(Visitor{}, std::move(value), std::move(element));
        }
        std::visit(top, std::move(value));
      }
    };
    template <flags::Flag Option>
    struct JoinedStr : seq<JoinedStrOne<Option>> {
      struct Transform
          : rules::Stack<std::string, asdl::JoinedStr, object::Object> {
        struct Push {
          using State = std::variant<asdl::JoinedStr, object::Object>;
          auto operator()(std::string && /*value*/) -> State { Expects(false); }
          auto operator()(asdl::JoinedStr &&value) {
            return State{std::move(value)};
          }
          auto operator()(object::Object &&value) {
            return State{std::move(value)};
          }
        };
        template <typename Outer>
        void success(Outer &&outer) {
          std::visit(outer, std::visit(Push{}, pop()));
        }
      };
    };
    template <flags::Flag Option>
    struct Action<JoinedStr<Option>> {
      struct Push {
        using State = std::variant<asdl::JoinedStr, object::Object>;
        auto operator()(std::string &&value) {
          return State{object::Object(object::String(value), {})};
        }
        auto operator()(asdl::JoinedStr &&value) {
          return State{std::move(value)};
        }
        auto operator()(object::Object &&value) {
          return State{std::move(value)};
        }
      };
      template <typename Top>
      static void apply0(Top &&top) {
        std::visit(top, std::visit(Push{}, top.pop()));
      }
    };
  } // namespace token
  template <flags::Flag Option>
  struct DocString : seq<token::DocString<Option>, sor<NEWLINE, at<Eolf>>> {
    using Transform = rules::ReshapeCapture<asdl::DocString, object::Object>;
  };
  template <flags::Flag Option>
  struct STRING : sor<token::Bytes<Option>, token::JoinedStr<Option>> {};
} // namespace chimera::library::grammar

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
