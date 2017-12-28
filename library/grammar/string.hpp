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

#include "grammar/control.hpp"
#include "grammar/exprfwd.hpp"
#include "grammar/input.hpp"
#include "grammar/whitespace.hpp"
#include "object/object.hpp"
#include "virtual_machine/process_context.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace string {
        using namespace std::literals;

        template <typename Rule>
        struct ChimeraActions : Nothing<Rule> {};
        struct BytesHolder : rules::Stack<asdl::ExprImpl> {
          object::Bytes bytes;
          template <typename Stack>
          void success(Stack &&stack) {
            stack.push(pop<asdl::ExprImpl>());
          }
          template <typename String>
          void apply(String &&in) {
            std::transform(in.begin(), in.end(), std::back_inserter(bytes),
                           [](const auto &byte) {
                             return static_cast<std::uint8_t>(byte);
                           });
          }
        };
        struct StringHolder : rules::Stack<asdl::ExprImpl> {
          std::string string;
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
          template <typename String>
          void apply(String &&in) {
            string.append(std::forward<String>(in));
          }
        };
        struct PartialStringHolder {
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
        struct LiteralChar : Plus<NotOne<'\0', '{', '}'>> {};
        template <>
        struct ChimeraActions<LiteralChar> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            top.apply(in.string());
          }
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct FExpression
            : Sor<Seq<Sor<InputRule<ConditionalExpression<true, AsyncFlow,
                                                          ScopeFlow>>,
                          Seq<One<'*'>,
                              InputRule<Expr<true, AsyncFlow, ScopeFlow>>>>,
                      Star<Sor<Seq<
                          Seq<One<','>, InputRule<ConditionalExpression<
                                            true, AsyncFlow, ScopeFlow>>>,
                          Seq<One<','>, One<'*'>,
                              InputRule<Expr<true, AsyncFlow, ScopeFlow>>>>>>,
                      Opt<One<','>>>,
                  InputRule<YieldExpr<true, AsyncFlow, ScopeFlow>>> {};
        struct Conversion : One<'a', 'r', 's'> {};
        template <>
        struct ChimeraActions<Conversion> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
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
        struct FormatSpec : Star<Sor<LiteralChar, Nul,
                                     ReplacementField<AsyncFlow, ScopeFlow>>> {
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct ChimeraActions<FormatSpec<AsyncFlow, ScopeFlow>> {
          template <typename ProcessContext, typename Top>
          static void apply0(ProcessContext && /*processContext*/, Top &&top) {
            auto formatSpec = top.template pop<asdl::ExprImpl>();
            [&formatSpec](auto &&expr) {
              if (std::holds_alternative<asdl::FormattedValue>(*expr.value)) {
                std::get<asdl::FormattedValue>(*expr.value).format_spec =
                    std::move(formatSpec);
              } else {
                asdl::FormattedValue formattedValue{
                    expr, asdl::FormattedValue::STR, std::move(formatSpec)};
                expr = std::move(formattedValue);
              }
            }(top.template top<asdl::ExprImpl>());
          }
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct ReplacementField
            : IfMust<One<'{'>, FExpression<AsyncFlow, ScopeFlow>,
                     Opt<One<'!'>, Conversion>,
                     Opt<One<':'>, FormatSpec<AsyncFlow, ScopeFlow>>,
                     One<'}'>> {};
        struct LeftFLiteral : String<'{', '{'> {};
        template <>
        struct ChimeraActions<LeftFLiteral> {
          template <typename ProcessContext, typename Top>
          static void apply0(ProcessContext && /*processContext*/, Top &&top) {
            top.apply("{"sv);
          }
        };
        struct RightFLiteral : String<'}', '}'> {};
        template <>
        struct ChimeraActions<RightFLiteral> {
          template <typename ProcessContext, typename Top>
          static void apply0(ProcessContext && /*processContext*/, Top &&top) {
            top.apply("}"sv);
          }
        };
        struct FLiteral : Plus<Sor<LiteralChar, LeftFLiteral, RightFLiteral>> {
          using Transform = StringHolder;
        };
        template <>
        struct ChimeraActions<FLiteral> {
          template <typename ProcessContext, typename Top>
          static void apply0(ProcessContext &&processContext, Top &&top) {
            top.push(asdl::ExprImpl{
                processContext.insert_constant(object::String(top.string))});
          }
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct FString
            : Must<Star<Sor<FLiteral, ReplacementField<AsyncFlow, ScopeFlow>>>,
                   Eof> {};
        struct FormattedStringHolder : rules::Stack<asdl::ExprImpl> {
          std::string string;
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 0) {
              asdl::JoinedStr joinedStr;
              joinedStr.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(joinedStr.values));
              outer.push(asdl::ExprImpl{std::move(joinedStr)});
            }
          }
          template <typename String>
          void apply(String &&in) {
            string.append(std::forward<String>(in));
          }
        };
        template <typename Chars>
        struct SingleChars : Plus<Chars> {};
        template <typename Chars>
        struct ChimeraActions<SingleChars<Chars>> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            top.apply(in.string());
          }
        };
        template <unsigned Len>
        struct Hexseq : Rep<Len, Xdigit> {};
        template <unsigned Len>
        struct ChimeraActions<Hexseq<Len>> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            std::string string;
            if (tao::pegtl::unescape::utf8_append_utf32(
                    string, tao::pegtl::unescape::unhex_string<std::uint32_t>(
                                in.begin(), in.end()))) {
              top.apply(std::move(string));
            }
          }
        };
        template <char Open, unsigned Len>
        struct UTF : Seq<One<Open>, Hexseq<Len>> {};
        struct Octseq : Seq<Range<'0', '7'>, RepOpt<2, Range<'0', '7'>>> {};
        template <>
        struct ChimeraActions<Octseq> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
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
        struct EscapeControl : One<'a', 'b', 'f', 'n', 'r', 't', 'v'> {};
        template <>
        struct ChimeraActions<EscapeControl> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
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
                Ensures(false);
            }
          }
        };
        template <typename Chars>
        struct EscapeIgnore : Seq<Chars> {};
        template <typename Chars>
        struct ChimeraActions<EscapeIgnore<Chars>> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            top.apply(R"(\)"sv);
            top.apply(in.string());
          }
        };
        struct Escape : One<'\\'> {};
        struct XEscapeseq : UTF<'x', 2> {};
        template <typename Chars, typename... Escapes>
        struct Escapeseq : Sor<Escapes..., XEscapeseq, Octseq, Eol,
                               EscapeControl, EscapeIgnore<Chars>> {};
        template <typename Chars, typename... Escapes>
        struct Item
            : Seq<rules::IfThenElse<Escape, Escapeseq<Chars, Escapes...>,
                                    SingleChars<Minus<Chars, Escape>>>,
                  Discard> {};
        template <typename Chars>
        struct RawItem : rules::IfThenElse<Escape, Chars, Chars> {};
        template <typename Triple, typename Chars, typename... Escapes>
        struct Long
            : IfMust<
                  Triple,
                  Until<Triple, Item<Seq<NotAt<Triple>, Chars>, Escapes...>>> {
        };
        template <typename Triple, typename Chars>
        struct LongRaw
            : IfMust<Triple,
                     Until<Triple, RawItem<Seq<NotAt<Triple>, Chars>>>> {};
        template <typename Triple, typename Chars>
        struct ChimeraActions<LongRaw<Triple, Chars>> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            std::string_view view(in.begin(), in.size());
            view.remove_prefix(3);
            view.remove_suffix(3);
            top.apply(view);
          }
        };
        template <typename Quote, typename Chars, typename... Escapes>
        struct Short
            : IfMust<Quote,
                     Until<Quote, Item<Minus<Seq<NotAt<Quote>, Chars>, Eol>,
                                       Escapes...>>> {};
        template <typename Quote, typename Chars>
        struct ShortRaw
            : IfMust<
                  Quote,
                  Until<Quote, RawItem<Minus<Seq<NotAt<Quote>, Chars>, Eol>>>> {
        };
        template <typename Quote, typename Chars>
        struct ChimeraActions<ShortRaw<Quote, Chars>> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            std::string_view view(in.begin(), in.size());
            view.remove_prefix(1);
            view.remove_suffix(1);
            top.apply(view);
          }
        };
        using TripleSingle = Rep<3, One<'\''>>;
        using TripleDouble = Rep<3, One<'"'>>;
        using Single = One<'\''>;
        using Double = One<'"'>;
        template <typename Chars>
        struct Raw
            : Sor<LongRaw<TripleDouble, Chars>, LongRaw<TripleSingle, Chars>,
                  ShortRaw<Double, Chars>, ShortRaw<Single, Chars>> {};
        template <typename Chars, typename... Escapes>
        struct Escaped : Sor<Long<TripleDouble, Chars, Escapes...>,
                             Long<TripleSingle, Chars, Escapes...>,
                             Short<Double, Chars, Escapes...>,
                             Short<Single, Chars, Escapes...>> {};
        struct UTF16Escape : UTF<'u', 4> {};
        struct UTF32Escape : UTF<'U', 8> {};
        struct UName : Star<NotOne<'}'>> {};
        template <>
        struct ChimeraActions<UName> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            top.apply(in.string());
          }
        };
        struct UNameEscape : Seq<One<'N'>, One<'{'>, UName, One<'}'>> {};
        template <typename Prefix, typename RawPrefix, typename Chars,
                  typename... Escapes>
        struct StringImpl : Plus<Sor<Seq<RawPrefix, Raw<Chars>>,
                                     Seq<Prefix, Escaped<Chars, Escapes...>>>> {
        };
        struct BytesPrefix : Sor<One<'b'>, One<'B'>> {};
        struct BytesRawPrefix
            : Sor<Seq<Sor<One<'r'>, One<'R'>>, Sor<One<'b'>, One<'B'>>>,
                  Seq<Sor<One<'b'>, One<'B'>>, Sor<One<'r'>, One<'R'>>>> {};
        template <bool Implicit>
        struct Bytes
            : Plus<Token<Implicit,
                         StringImpl<BytesPrefix, BytesRawPrefix, Seven>>> {
          using Transform = BytesHolder;
        };
        template <bool Implicit>
        struct ChimeraActions<Bytes<Implicit>> {
          template <typename ProcessContext, typename Top>
          static void apply0(ProcessContext &&processContext, Top &&top) {
            top.push(asdl::ExprImpl{
                processContext.insert_constant(std::move(top.bytes))});
          }
        };
        struct StrPrefix : Opt<Sor<One<'u'>, One<'U'>>> {};
        struct StrRawPrefix : Sor<One<'r'>, One<'R'>> {};
        template <bool Implicit>
        struct String
            : Plus<Token<Implicit,
                         StringImpl<StrPrefix, StrRawPrefix, Any, UTF16Escape,
                                    UTF32Escape, UNameEscape>>> {
          using Transform = StringHolder;
        };
        template <bool Implicit>
        struct ChimeraActions<String<Implicit>> {
          template <typename ProcessContext, typename Top>
          static void apply0(ProcessContext &&processContext, Top &&top) {
            top.push(asdl::ExprImpl{
                processContext.insert_constant(object::String(top.string))});
          }
        };
        struct JoinedStrPrefix : Sor<One<'f'>, One<'F'>> {};
        struct JoinedStrRawPrefix
            : Sor<Seq<Sor<One<'r'>, One<'R'>>, Sor<One<'f'>, One<'F'>>>,
                  Seq<Sor<One<'f'>, One<'F'>>, Sor<One<'r'>, One<'R'>>>> {};
        struct PartialString
            : Plus<StringImpl<StrPrefix, StrRawPrefix, Any, UTF16Escape,
                              UTF32Escape, UNameEscape>> {
          using Transform = PartialStringHolder;
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct FormattedString
            : Seq<StringImpl<JoinedStrPrefix, JoinedStrRawPrefix, Any,
                             UTF16Escape, UTF32Escape, UNameEscape>> {
          using Transform = FormattedStringHolder;
        };
        template <bool AsyncFlow, bool ScopeFlow>
        struct ChimeraActions<FormattedString<AsyncFlow, ScopeFlow>> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in, ProcessContext &&processContext,
                            Top &&top) {
            Ensures((tao::pegtl::parse_nested<FString<AsyncFlow, ScopeFlow>,
                                              ChimeraActions, ControlBase>(
                in,
                tao::pegtl::memory_input<>(top.string.c_str(),
                                           top.string.size(), "<f_string>"),
                processContext, top)));
          }
        };
        template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
        struct JoinedStr
            : Plus<
                  Token<Implicit, Sor<PartialString,
                                      FormattedString<AsyncFlow, ScopeFlow>>>> {
          struct Transform : rules::Stack<std::string, asdl::ExprImpl> {
            template <typename Outer>
            void success(Outer &&outer) {
              outer.push(pop<asdl::ExprImpl>());
            }
          };
        };
        template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
        struct ChimeraActions<JoinedStr<Implicit, AsyncFlow, ScopeFlow>> {
          using State = std::variant<std::string, asdl::JoinedStr>;
          struct Visitor {
            virtual_machine::ProcessContext &process_context;
            State operator()(std::string &&value, const std::string &element) {
              value.append(element);
              return value;
            }
            State operator()(std::string &&value,
                             const asdl::ExprImpl &element) {
              auto &joinedStr = std::get<asdl::JoinedStr>(*element.value);
              joinedStr.values.emplace(
                  joinedStr.values.begin(),
                  process_context.insert_constant(object::String(value)));
              return State{std::move(joinedStr)};
            }
            State operator()(asdl::JoinedStr &&value,
                             const std::string &element) {
              value.values.emplace_back(
                  process_context.insert_constant(object::String(element)));
              return State{std::move(value)};
            }
            State operator()(asdl::JoinedStr &&value,
                             const asdl::ExprImpl &element) {
              auto &joinedStr = std::get<asdl::JoinedStr>(*element.value);
              std::move(joinedStr.values.begin(), joinedStr.values.end(),
                        std::back_inserter(value.values));
              return State{std::move(value)};
            }
          };
          struct Push {
            virtual_machine::ProcessContext &process_context;
            asdl::ExprImpl operator()(std::string &&value) {
              return asdl::ExprImpl{
                  process_context.insert_constant(object::String(value))};
            }
            asdl::ExprImpl operator()(asdl::JoinedStr &&value) {
              return asdl::ExprImpl{std::move(value)};
            }
          };
          template <typename ProcessContext, typename Top>
          static void apply0(ProcessContext &&processContext, Top &&top) {
            State value;
            for (auto &&element : top.vector()) {
              value = std::visit(Visitor{processContext}, std::move(value),
                                 element);
            }
            top.push(std::visit(Push{processContext}, std::move(value)));
          }
        };
      } // namespace string
      template <bool Implicit>
      struct DocString
          : Seq<Action<string::ChimeraActions, string::String<Implicit>>,
                Sor<NEWLINE, At<Eolf>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::DocString{
                std::get<asdl::Constant>(*pop<asdl::ExprImpl>().value)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct STRING
          : Action<string::ChimeraActions,
                   Sor<string::Bytes<Implicit>,
                       string::JoinedStr<Implicit, AsyncFlow, ScopeFlow>>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
