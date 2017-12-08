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
#include <tao/pegtl/contrib/changes.hpp>
#include <tao/pegtl/contrib/unescape.hpp>

#include "grammar/control.hpp"
#include "grammar/exprfwd.hpp"
#include "grammar/input.hpp"
#include "grammar/whitespace.hpp"
#include "object/object.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace string {
        template <typename Rule>
        struct Actions : tao::pegtl::nothing<Rule> {};
        template <typename Rule>
        struct Control : ControlBase<Rule> {};
        struct BytesHolder {
          object::Bytes bytes;
          std::size_t object_ref = 0;
          template <typename Stack>
          void success(Stack &&stack) {
            stack.push(asdl::ExprImpl{asdl::Constant{object_ref}});
          }
          template <typename String>
          void apply(String &&in) {
            for (const auto &byte : in) {
              bytes.push_back(static_cast<std::uint8_t>(byte));
            }
          }
        };
        struct StringHolder {
          std::string string;
          std::size_t object_ref = 0;
          template <typename Stack>
          void success(Stack &&stack) {
            stack.push(asdl::ExprImpl{asdl::Constant{object_ref}});
          }
          template <typename String>
          void apply(String &&in) {
            string.append(std::forward<String>(in));
          }
        };
        struct StringCreate : tao::pegtl::success {
          template <tao::pegtl::apply_mode A, tao::pegtl::rewind_mode M,
                    template <typename...> class Action,
                    template <typename...> class Control, typename Input,
                    typename Top>
          static bool match(Input &&in, Top &&top) {
            top.object_ref = in.constants.size();
            in.constants.emplace_back(
                object::Object(object::String(top.string), {}));
            return true;
          }
        };
        struct LiteralChar
            : tao::pegtl::plus<tao::pegtl::utf8::not_one<'\0', '{', '}'>> {};
        template <>
        struct Actions<LiteralChar> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.apply(in.string());
          }
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct FExpression
            : tao::pegtl::sor<
                  tao::pegtl::seq<
                      tao::pegtl::sor<
                          InputRule<ConditionalExpression<true, AsyncFlow,
                                                          ScopeFlow>>,
                          tao::pegtl::seq<
                              tao::pegtl::one<'*'>,
                              InputRule<Expr<true, AsyncFlow, ScopeFlow>>>>,
                      tao::pegtl::star<tao::pegtl::sor<tao::pegtl::seq<
                          tao::pegtl::seq<tao::pegtl::one<','>,
                                          InputRule<ConditionalExpression<
                                              true, AsyncFlow, ScopeFlow>>>,
                          tao::pegtl::seq<
                              tao::pegtl::one<','>, tao::pegtl::one<'*'>,
                              InputRule<Expr<true, AsyncFlow, ScopeFlow>>>>>>,
                      tao::pegtl::opt<tao::pegtl::one<','>>>,
                  InputRule<YieldExpr<true, AsyncFlow, ScopeFlow>>> {};
        struct Conversion : tao::pegtl::one<'a', 'r', 's'> {};
        template <>
        struct Actions<Conversion> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            asdl::FormattedValue formattedValue{
                top.template pop<asdl::ExprImpl>(),
                asdl::FormattedValue::STR,
                {}};
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
            top.push(asdl::ExprImpl{std::move(formattedValue)});
          }
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct ReplacementField;
        template <bool AsyncFlow, bool ScopeFlow>
        struct FormatSpec
            : tao::pegtl::star<
                  tao::pegtl::sor<LiteralChar, tao::pegtl::nul,
                                  ReplacementField<AsyncFlow, ScopeFlow>>> {};
        template <bool AsyncFlow, bool ScopeFlow>
        struct Actions<FormatSpec<AsyncFlow, ScopeFlow>> {
          template <typename Top>
          static void apply0(Top &&top) {
            auto formatSpec = top.template pop<asdl::ExprImpl>();
            [&formatSpec](auto &&expr) {
              if (std::holds_alternative<asdl::FormattedValue>(*expr.value)) {
                std::get<asdl::FormattedValue>(*expr.value).format_spec =
                    std::move(formatSpec);
              } else {
                asdl::FormattedValue formattedValue{
                    {}, asdl::FormattedValue::STR, std::move(formatSpec)};
                using std::swap;
                swap(expr, formattedValue.value);
                expr = std::move(formattedValue);
              }
            }(top.template top<asdl::ExprImpl>());
          }
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct ReplacementField
            : tao::pegtl::if_must<
                  tao::pegtl::one<'{'>, FExpression<AsyncFlow, ScopeFlow>,
                  tao::pegtl::opt<tao::pegtl::one<'!'>, Conversion>,
                  tao::pegtl::opt<tao::pegtl::one<':'>,
                                  FormatSpec<AsyncFlow, ScopeFlow>>,
                  tao::pegtl::one<'}'>> {};
        struct LeftFLiteral : tao::pegtl::two<'{'> {};
        template <>
        struct Actions<LeftFLiteral> {
          template <typename Input, typename Top>
          static void apply(const Input & /*in*/, Top &&top) {
            top.apply(std::string("{", 1));
          }
        };
        struct RightFLiteral : tao::pegtl::two<'}'> {};
        template <>
        struct Actions<RightFLiteral> {
          template <typename Input, typename Top>
          static void apply(const Input & /*in*/, Top &&top) {
            top.apply(std::string("}", 1));
          }
        };
        struct FLiteral
            : tao::pegtl::seq<tao::pegtl::plus<tao::pegtl::sor<
                                  LiteralChar, LeftFLiteral, RightFLiteral>>,
                              StringCreate> {};
        template <>
        struct Control<FLiteral> : ChangeState<FLiteral, StringHolder> {};
        template <bool AsyncFlow, bool ScopeFlow>
        struct FString
            : tao::pegtl::star<tao::pegtl::sor<
                  FLiteral, ReplacementField<AsyncFlow, ScopeFlow>>> {};
        template <bool AsyncFlow, bool ScopeFlow>
        struct JoinedStringHolder : Stack<asdl::ExprImpl> {
          object::String string;
          template <typename String>
          void apply(String &&in) {
            string.append(std::forward<String>(in));
          }
          template <typename Outer>
          void success(Outer &&outer) {
            std::visit(outer, pop());
          }
        };
        template <typename Chars>
        struct SingleChars : tao::pegtl::plus<Chars> {};
        template <typename Chars>
        struct Actions<SingleChars<Chars>> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.apply(in.string());
          }
        };
        template <unsigned Len>
        struct Hexseq : tao::pegtl::rep<Len, tao::pegtl::xdigit> {};
        template <unsigned Len>
        struct Actions<Hexseq<Len>> {
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
        struct UTF : tao::pegtl::seq<tao::pegtl::one<Open>, Hexseq<Len>> {};
        struct Octseq
            : tao::pegtl::seq<
                  tao::pegtl::range<'0', '7'>,
                  tao::pegtl::rep_opt<2, tao::pegtl::range<'0', '7'>>> {};
        template <>
        struct Actions<Octseq> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            std::string string;
            if (tao::pegtl::unescape::utf8_append_utf32(
                    string, std::accumulate(
                                in.begin(), in.end(), std::uint32_t(0),
                                [](const auto init, const auto c) {
                                  return (init << 2) |
                                         static_cast<std::uint32_t>(c - '0');
                                }))) {
              top.apply(std::move(string));
            }
          }
        };
        struct EscapeControl
            : tao::pegtl::one<'a', 'b', 'f', 'n', 'r', 't', 'v'> {};
        template <>
        struct Actions<EscapeControl> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            switch (in.peek_char()) {
              case 'a':
                top.apply(std::string("\a", 1));
                break;
              case 'b':
                top.apply(std::string("\b", 1));
                break;
              case 'f':
                top.apply(std::string("\f", 1));
                break;
              case 'n':
                top.apply(std::string("\n", 1));
                break;
              case 'r':
                top.apply(std::string("\r", 1));
                break;
              case 't':
                top.apply(std::string("\t", 1));
                break;
              case 'v':
                top.apply(std::string("\v", 1));
                break;
              default:
                Ensures(false);
            }
          }
        };
        template <typename Chars>
        struct EscapeIgnore : tao::pegtl::seq<Chars> {};
        template <typename Chars>
        struct Actions<EscapeIgnore<Chars>> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.apply(std::string(R"(\)", 1));
            top.apply(in.string());
          }
        };
        struct Escape : tao::pegtl::one<'\\'> {};
        struct XEscapeseq : UTF<'x', 2> {};
        template <typename Chars, typename... Escapes>
        struct Escapeseq
            : tao::pegtl::sor<Escapes..., XEscapeseq, Octseq, tao::pegtl::eol,
                              EscapeControl, EscapeIgnore<Chars>> {};
        template <typename Chars, typename... Escapes>
        struct Item : tao::pegtl::if_then_else<
                          Escape, Escapeseq<Chars, Escapes...>,
                          SingleChars<tao::pegtl::minus<Chars, Escape>>> {};
        template <typename Chars>
        struct RawItem : tao::pegtl::if_then_else<Escape, Chars, Chars> {};
        template <typename Triple, typename Chars, typename... Escapes>
        struct Long
            : tao::pegtl::if_must<
                  Triple,
                  tao::pegtl::until<
                      Triple,
                      Item<tao::pegtl::seq<tao::pegtl::not_at<Triple>, Chars>,
                           Escapes...>>> {};
        template <typename Triple, typename Chars>
        struct LongRaw
            : tao::pegtl::if_must<
                  Triple,
                  tao::pegtl::until<Triple,
                                    RawItem<tao::pegtl::seq<
                                        tao::pegtl::not_at<Triple>, Chars>>>> {
        };
        template <typename Triple, typename Chars>
        struct Actions<LongRaw<Triple, Chars>> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            std::string_view view(in.begin(), in.size());
            view.remove_prefix(3);
            view.remove_suffix(3);
            top.apply(view);
          }
        };
        template <typename Quote, typename Chars, typename... Escapes>
        struct Short
            : tao::pegtl::if_must<
                  Quote,
                  tao::pegtl::until<
                      Quote, Item<tao::pegtl::minus<
                                      tao::pegtl::seq<tao::pegtl::not_at<Quote>,
                                                      Chars>,
                                      tao::pegtl::eol>,
                                  Escapes...>>> {};
        template <typename Quote, typename Chars>
        struct ShortRaw
            : tao::pegtl::if_must<
                  Quote,
                  tao::pegtl::until<
                      Quote,
                      RawItem<tao::pegtl::minus<
                          tao::pegtl::seq<tao::pegtl::not_at<Quote>, Chars>,
                          tao::pegtl::eol>>>> {};
        template <typename Quote, typename Chars>
        struct Actions<ShortRaw<Quote, Chars>> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            std::string_view view(in.begin(), in.size());
            view.remove_prefix(1);
            view.remove_suffix(1);
            top.apply(view);
          }
        };
        using TripleSingle = tao::pegtl::rep<3, tao::pegtl::one<'\''>>;
        using TripleDouble = tao::pegtl::rep<3, tao::pegtl::one<'"'>>;
        using Single = tao::pegtl::one<'\''>;
        using Double = tao::pegtl::one<'"'>;
        template <typename Chars>
        struct Raw
            : tao::pegtl::sor<
                  LongRaw<TripleDouble, Chars>, LongRaw<TripleSingle, Chars>,
                  ShortRaw<Double, Chars>, ShortRaw<Single, Chars>> {};
        template <typename Chars, typename... Escapes>
        struct Escaped : tao::pegtl::sor<Long<TripleDouble, Chars, Escapes...>,
                                         Long<TripleSingle, Chars, Escapes...>,
                                         Short<Double, Chars, Escapes...>,
                                         Short<Single, Chars, Escapes...>> {};
        struct UTF16Escape : UTF<'u', 4> {};
        struct UTF32Escape : UTF<'U', 8> {};
        struct UName : tao::pegtl::star<tao::pegtl::utf8::not_one<'}'>> {};
        template <>
        struct Actions<UName> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.apply(in.string());
          }
        };
        struct UNameEscape
            : tao::pegtl::seq<tao::pegtl::one<'N'>, tao::pegtl::one<'{'>, UName,
                              tao::pegtl::one<'}'>> {};
        template <typename Prefix, typename RawPrefix, typename Chars,
                  typename... Escapes>
        struct StringImpl
            : tao::pegtl::plus<tao::pegtl::sor<
                  tao::pegtl::seq<RawPrefix, Raw<Chars>>,
                  tao::pegtl::seq<Prefix, Escaped<Chars, Escapes...>>>> {};
        struct BytesPrefix : tao::pegtl::istring<'b'> {};
        struct BytesRawPrefix : tao::pegtl::sor<tao::pegtl::istring<'r', 'b'>,
                                                tao::pegtl::istring<'b', 'r'>> {
        };
        struct BytesCreate : tao::pegtl::success {
          template <tao::pegtl::apply_mode A, tao::pegtl::rewind_mode M,
                    template <typename...> class Action,
                    template <typename...> class Control, typename Input,
                    typename Top>
          static bool match(Input &&in, Top &&top) {
            top.object_ref = in.constants.size();
            in.constants.emplace_back(object::Object(std::move(top.bytes), {}));
            return true;
          }
        };
        struct Bytes
            : tao::pegtl::seq<
                  StringImpl<BytesPrefix, BytesRawPrefix, tao::pegtl::seven>,
                  BytesCreate> {};
        template <>
        struct Control<Bytes> : ChangeState<Bytes, BytesHolder> {};
        struct StrPrefix : tao::pegtl::opt<tao::pegtl::istring<'u'>> {};
        struct StrRawPrefix : tao::pegtl::istring<'r'> {};
        struct String
            : tao::pegtl::seq<
                  StringImpl<StrPrefix, StrRawPrefix, tao::pegtl::utf8::any,
                             UTF16Escape, UTF32Escape, UNameEscape>,
                  StringCreate> {};
        template <>
        struct Control<String> : ChangeState<String, StringHolder> {};
        struct JoinedStrPrefix : tao::pegtl::istring<'f'> {};
        struct JoinedStrRawPrefix
            : tao::pegtl::sor<tao::pegtl::istring<'r', 'f'>,
                              tao::pegtl::istring<'f', 'r'>> {};
        template <bool AsyncFlow, bool ScopeFlow>
        struct JoinedStrCreate : tao::pegtl::success {
          template <tao::pegtl::apply_mode A, tao::pegtl::rewind_mode M,
                    template <typename...> class Action,
                    template <typename...> class Control, typename Input,
                    typename Top>
          static bool match(Input &&in, Top &&top) {
            if (!tao::pegtl::parse_nested<
                    tao::pegtl::must<FString<AsyncFlow, ScopeFlow>,
                                     tao::pegtl::eof>,
                    Action, Control>(
                    in,
                    library::grammar::Input<tao::pegtl::memory_input<>>(
                        in.constants, top.string.c_str(), "<f_string>"),
                    top)) {
              return false;
            }
            if (auto size = top.size(); size > 0) {
              if (size == 1) {
                return true;
              }
              asdl::JoinedStr joinedStr;
              joinedStr.values.resize(size);
              top.transform(joinedStr.values.begin());
              top.push(asdl::ExprImpl{std::move(joinedStr)});
            } else {
              top.push(asdl::ExprImpl{asdl::Constant{in.constants.size()}});
              in.constants.emplace_back(object::Object(object::String{}, {}));
            }
            return true;
          }
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct JoinedStr
            : tao::pegtl::seq<StringImpl<JoinedStrPrefix, JoinedStrRawPrefix,
                                         tao::pegtl::utf8::any, UTF16Escape,
                                         UTF32Escape, UNameEscape>,
                              JoinedStrCreate<AsyncFlow, ScopeFlow>> {};
        template <bool AsyncFlow, bool ScopeFlow>
        struct Control<JoinedStr<AsyncFlow, ScopeFlow>>
            : ChangeState<JoinedStr<AsyncFlow, ScopeFlow>,
                          JoinedStringHolder<AsyncFlow, ScopeFlow>> {};
      } // namespace string
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct STRING
          : tao::pegtl::action<
                string::Actions,
                tao::pegtl::control<
                    string::Control,
                    tao::pegtl::sor<
                        tao::pegtl::plus<Token<Implicit, string::Bytes>>,
                        tao::pegtl::plus<Token<
                            Implicit, tao::pegtl::sor<string::JoinedStr<
                                                          AsyncFlow, ScopeFlow>,
                                                      string::String>>>>>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
