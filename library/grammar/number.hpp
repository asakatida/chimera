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

//! parse definitions for number tokens.

#pragma once

#include <cstdint>
#include <numeric>
#include <vector>

#include <tao/pegtl.hpp>

#include "asdl/asdl.hpp"
#include "grammar/control.hpp"
#include "grammar/whitespace.hpp"
#include "object/object.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace number {
        template <typename Rule>
        struct ChimeraActions : Nothing<Rule> {};
        template <typename... Args>
        object::Number ston(Args &&... args) {
          return object::number::number(
              std::stoul(std::forward<Args>(args)...));
        }
        struct Holder : rules::Stack<asdl::ExprImpl> {
          object::Number number = object::number::number(0);
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            number = number * object::number::number(Base).pow(
                                  object::number::number(in.size())) +
                     ston(in.string(), nullptr, Base);
          }
        };
        struct FloatHolder {
          object::Number numerator = object::number::number(0);
          object::Number denominator = object::number::number(1);
          template <typename Top>
          void success(Top &&top) {
            top.object =
                numerator / object::number::number(10).pow(denominator) +
                top.object;
          }
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            auto size = object::number::number(in.size());
            denominator = denominator + size;
            numerator = numerator * object::number::number(Base).pow(size) +
                        ston(in.string(), nullptr, Base);
          }
        };
        struct ExponentHolder {
          object::Number number = object::number::number(0);
          template <typename Top>
          void success(Top &&top) {
            top.number = object::number::number(10).pow(number) * top.number;
          }
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            number = number * object::number::number(Base).pow(
                                  object::number::number(in.size())) +
                     ston(in.string(), nullptr, Base);
          }
        };
        struct ImaginaryHolder {
          object::Number number = object::number::number(0);
          template <typename Top>
          void success(Top &&top) {
            top.number = number.complex();
          }
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            number = number * object::number::number(Base).pow(
                                  object::number::number(in.size())) +
                     ston(in.string(), nullptr, Base);
          }
        };
        struct Nonzerodigit : Seq<Range<'1', '9'>, RepOpt<18, Digit>> {};
        template <>
        struct ChimeraActions<Nonzerodigit> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            top.template apply<10>(in);
          }
        };
        struct Digit : Seq<Digit, RepOpt<18, Digit>> {};
        template <>
        struct ChimeraActions<Digit> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            top.template apply<10>(in);
          }
        };
        struct Bindigit : Seq<Range<'0', '1'>, RepOpt<63, Range<'0', '1'>>> {};
        template <>
        struct ChimeraActions<Bindigit> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            top.template apply<2>(in);
          }
        };
        struct Octdigit : Seq<Range<'0', '7'>, RepOpt<31, Range<'0', '7'>>> {};
        template <>
        struct ChimeraActions<Octdigit> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            top.template apply<8>(in);
          }
        };
        struct Hexdigit : Seq<Xdigit, RepOpt<15, Xdigit>> {};
        template <>
        struct ChimeraActions<Hexdigit> {
          template <typename Input, typename ProcessContext, typename Top>
          static void apply(const Input &in,
                            ProcessContext && /*processContext*/, Top &&top) {
            top.template apply<16>(in);
          }
        };
        using DecIntegerNonZeroDigit =
            Seq<Nonzerodigit, Star<Opt<One<'_'>>, Digit>>;
        using DecIntegerZeroDigit =
            Seq<One<'0'>, Star<Opt<One<'_'>>, One<'0'>>>;
        using Decinteger = Sor<DecIntegerNonZeroDigit, DecIntegerZeroDigit>;
        using BinStart = Seq<One<'0'>, Sor<One<'b'>, One<'B'>>>;
        using Bininteger = IfMust<BinStart, Plus<Opt<One<'_'>>, Bindigit>>;
        using OctStart = Seq<One<'0'>, Sor<One<'o'>, One<'O'>>>;
        using Octinteger = IfMust<OctStart, Plus<Opt<One<'_'>>, Octdigit>>;
        using HexStart = Seq<One<'0'>, Sor<One<'x'>, One<'X'>>>;
        using Hexinteger = IfMust<HexStart, Plus<Opt<One<'_'>>, Hexdigit>>;
        using Integer = Sor<Bininteger, Octinteger, Hexinteger, Decinteger>;

        using Digitpart = Plus<Opt<One<'_'>>, Digit>;
        using Fraction = Seq<One<'.'>, Digitpart>;
        struct ExponentNegative : Seq<One<'-'>, Digitpart> {};
        template <>
        struct ChimeraActions<ExponentNegative> {
          template <typename ProcessContext, typename Top>
          static void apply0(ProcessContext && /*processContext*/, Top &&top) {
            top.number = -top.number;
          }
        };
        using Exponent =
            IfMust<One<'e', 'E'>,
                   Sor<Seq<Opt<One<'+'>>, Digitpart>, ExponentNegative>>;
        using Pointfloat =
            Sor<Seq<Opt<Digitpart>, Fraction>, Seq<Digitpart, One<'.'>>>;
        using Exponentfloat = Seq<Sor<Digitpart, Pointfloat>, Exponent>;
        using Floatnumber = Sor<Pointfloat, Exponentfloat>;
        struct Imagnumber : Seq<Sor<Floatnumber, Digitpart>, One<'j', 'J'>> {
          using Transform = ImaginaryHolder;
        };
        struct Numberliteral : Sor<Imagnumber, Floatnumber, Integer> {
          using Transform = Holder;
        };
        template <>
        struct ChimeraActions<Numberliteral> {
          template <typename ProcessContext, typename Top>
          static void apply0(ProcessContext &&processContext, Top &&top) {
            top.push(asdl::ExprImpl{
                processContext.insert_constant(std::move(top.number))});
          }
        };
      } // namespace number
      template <bool Implicit>
      struct NUMBER
          : Token<Implicit,
                  Action<number::ChimeraActions, number::Numberliteral>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
