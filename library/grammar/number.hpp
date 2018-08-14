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
      namespace token {
        struct NumberHolder {
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            number *= object::Number(Base).pow(object::Number(in.size())) +
                      object::Number(std::stoul(in.string(), nullptr, Base));
          }

          object::Number number = object::Number(0u);
        };
        struct Nonzerodigit
            : seq<range<'1', '9'>, rep_opt<18, range<'0', '9'>>> {};
        template <>
        struct Action<Nonzerodigit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<10>(in);
          }
        };
        struct Digit : seq<range<'0', '9'>, rep_opt<18, range<'0', '9'>>> {};
        template <>
        struct Action<Digit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<10>(in);
          }
        };
        struct Bindigit : seq<range<'0', '1'>, rep_opt<63, range<'0', '1'>>> {};
        template <>
        struct Action<Bindigit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<2>(in);
          }
        };
        struct Octdigit : seq<range<'0', '7'>, rep_opt<31, range<'0', '7'>>> {};
        template <>
        struct Action<Octdigit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<8>(in);
          }
        };
        struct Hexdigit
            : seq<ranges<'0', '9', 'a', 'f', 'A', 'F'>,
                  rep_opt<15, ranges<'0', '9', 'a', 'f', 'A', 'F'>>> {};
        template <>
        struct Action<Hexdigit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<16>(in);
          }
        };
        using DecIntegerNonZeroDigit =
            seq<Nonzerodigit, star<opt<one<'_'>>, Digit>>;
        using DecIntegerZeroDigit =
            seq<one<'0'>, star<opt<one<'_'>>, one<'0'>>>;
        using Decinteger = sor<DecIntegerNonZeroDigit, DecIntegerZeroDigit>;
        using BinStart = seq<one<'0'>, one<'b', 'B'>>;
        using Bininteger = if_must<BinStart, plus<opt<one<'_'>>, Bindigit>>;
        using OctStart = seq<one<'0'>, one<'o', 'O'>>;
        using Octinteger = if_must<OctStart, plus<opt<one<'_'>>, Octdigit>>;
        using HexStart = seq<one<'0'>, one<'x', 'X'>>;
        using Hexinteger = if_must<HexStart, plus<opt<one<'_'>>, Hexdigit>>;
        using Integer = sor<Bininteger, Octinteger, Hexinteger, Decinteger>;

        using Digitpart = plus<opt<one<'_'>>, Digit>;
        using Fraction = seq<one<'.'>, Digitpart>;
        struct ExponentNegative : seq<one<'-'>, Digitpart> {
          struct Transform : NumberHolder {
            template <typename Top>
            void success(Top &&top) {
              top.number = -number;
            }
          };
        };
        struct Exponent
            : if_must<one<'e', 'E'>,
                      sor<seq<opt<one<'+'>>, Digitpart>, ExponentNegative>> {
          struct Transform : NumberHolder {
            template <typename Top>
            void success(Top &&top) {
              top.number *= object::Number(10).pow(number);
            }
          };
        };
        using Pointfloat =
            sor<seq<opt<Digitpart>, Fraction>, seq<Digitpart, one<'.'>>>;
        using Exponentfloat = seq<sor<Digitpart, Pointfloat>, Exponent>;
        struct Floatnumber : sor<Pointfloat, Exponentfloat> {
          struct Transform : NumberHolder {
            object::Number denominator = object::Number(1);
            template <typename Top>
            void success(Top &&top) {
              top.number += number / object::Number(10).pow(denominator);
            }
            template <std::uint8_t Base, typename Input>
            void apply(const Input &in) {
              denominator += object::Number(in.size());
              NumberHolder::apply<Base>(in);
            }
          };
        };
        struct Imagnumber : seq<sor<Floatnumber, Digitpart>, one<'j', 'J'>> {
          struct Transform : NumberHolder {
            template <typename Top>
            void success(Top &&top) {
              top.number = number.complex();
            }
          };
        };
        struct Numberliteral : sor<Imagnumber, Floatnumber, Integer> {
          struct Transform : NumberHolder,
                             rules::VariantCapture<asdl::ExprImpl> {};
        };
        template <>
        struct Action<Numberliteral> {
          template <typename Top>
          static void apply0(Top &&top) {
            top.push(object::Object(std::move(top.number), {}));
          }
        };
      } // namespace token
      template <flags::Flag Option>
      struct NUMBER : token::Token<Option, token::Numberliteral> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
