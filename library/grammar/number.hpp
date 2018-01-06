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

#include "asdl/asdl.hpp"
#include "grammar/rules.hpp"
#include "grammar/whitespace.hpp"
#include "object/object.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace number {
        template <typename Rule>
        struct NumberActions : Nothing<Rule> {};
        struct NumberHolder {
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            number =
                number * object::number::number(Base).pow(
                             object::number::number(in.size())) +
                object::number::number(std::stoul(in.string(), nullptr, Base));
          }

        public:
          object::Number number = object::number::number(0);
        };
        struct Nonzerodigit : Seq<Range<'1', '9'>, RepOpt<18, Digit>> {};
        template <>
        struct NumberActions<Nonzerodigit> {
          template <typename Input, typename Top, typename... Args>
          static void apply(const Input &in, Top &&top,
                            const Args &... /*args*/) {
            top.template apply<10>(in);
          }
        };
        struct Digit : Seq<Digit, RepOpt<18, Digit>> {};
        template <>
        struct NumberActions<Digit> {
          template <typename Input, typename Top, typename... Args>
          static void apply(const Input &in, Top &&top,
                            const Args &... /*args*/) {
            top.template apply<10>(in);
          }
        };
        struct Bindigit : Seq<Range<'0', '1'>, RepOpt<63, Range<'0', '1'>>> {};
        template <>
        struct NumberActions<Bindigit> {
          template <typename Input, typename Top, typename... Args>
          static void apply(const Input &in, Top &&top,
                            const Args &... /*args*/) {
            top.template apply<2>(in);
          }
        };
        struct Octdigit : Seq<Range<'0', '7'>, RepOpt<31, Range<'0', '7'>>> {};
        template <>
        struct NumberActions<Octdigit> {
          template <typename Input, typename Top, typename... Args>
          static void apply(const Input &in, Top &&top,
                            const Args &... /*args*/) {
            top.template apply<8>(in);
          }
        };
        struct Hexdigit : Seq<Xdigit, RepOpt<15, Xdigit>> {};
        template <>
        struct NumberActions<Hexdigit> {
          template <typename Input, typename Top, typename... Args>
          static void apply(const Input &in, Top &&top,
                            const Args &... /*args*/) {
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
        struct ExponentNegative : Seq<One<'-'>, Digitpart> {
          struct Transform : NumberHolder {
            template <typename Top>
            void success(Top &&top) {
              top.number = -number;
            }
          };
        };
        struct Exponent
            : IfMust<One<'e', 'E'>,
                     Sor<Seq<Opt<One<'+'>>, Digitpart>, ExponentNegative>> {
          struct Transform : NumberHolder {
            template <typename Top>
            void success(Top &&top) {
              top.number = object::number::number(10).pow(number) * top.number;
            }
          };
        };
        using Pointfloat =
            Sor<Seq<Opt<Digitpart>, Fraction>, Seq<Digitpart, One<'.'>>>;
        using Exponentfloat = Seq<Sor<Digitpart, Pointfloat>, Exponent>;
        struct Floatnumber : Sor<Pointfloat, Exponentfloat> {
          struct Transform : NumberHolder {
            object::Number denominator = object::number::number(1);
            template <typename Top>
            void success(Top &&top) {
              top.number =
                  number / object::number::number(10).pow(denominator) +
                  top.number;
            }
            template <std::uint8_t Base, typename Input>
            void apply(const Input &in) {
              denominator = denominator + object::number::number(in.size());
              NumberHolder::apply<Base>(in);
            }
          };
        };
        struct Imagnumber : Seq<Sor<Floatnumber, Digitpart>, One<'j', 'J'>> {
          struct Transform : NumberHolder {
            template <typename Top>
            void success(Top &&top) {
              top.number = number.complex();
            }
          };
        };
        struct Numberliteral : Sor<Imagnumber, Floatnumber, Integer> {
          struct Transform : NumberHolder, rules::Stack<asdl::ExprImpl> {
            template <typename Outer>
            void success(Outer &&outer) {
              outer.push(pop<asdl::ExprImpl>());
            }
          };
        };
        template <>
        struct NumberActions<Numberliteral> {
          template <typename Top, typename ProcessContext>
          static void apply0(Top &&top, ProcessContext &&processContext) {
            top.push(asdl::ExprImpl{
                processContext.insert_constant(std::move(top.number))});
          }
        };
      } // namespace number
      template <typename Option>
      using NUMBER =
          Token<Option, Action<number::NumberActions, number::Numberliteral>>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
