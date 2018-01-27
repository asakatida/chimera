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

//! parse definitions for operator tokens.

#pragma once

#include <type_traits>

#include "asdl/asdl.hpp"
#include "grammar/rules.hpp"
#include "grammar/whitespace.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace token {
        namespace oper {
          template <auto Asdl>
          struct Transform {
            template <typename Outer>
            void success(Outer &&outer) {
              outer.push(Asdl);
            }
          };
        } // namespace oper
        template <flags::Flag Option, typename Rule, char... Cs>
        using Op =
            Token<Option,
                  std::conditional_t<(sizeof...(Cs) > 0),
                                     seq<Rule, not_at<one<Cs...>>>, Rule>>;
        template <flags::Flag Option>
        struct Add : Op<Option, one<'+'>, '='> {
          using Transform = oper::Transform<asdl::Operator::ADD>;
        };
        template <flags::Flag Option>
        using Arr = Op<Option, String<'-', '>'>>;
        template <flags::Flag Option>
        using AtImpl = Op<Option, one<'@'>, '='>;
        template <flags::Flag Option>
        using AtOp = seq<AtImpl<Option>>;
        template <flags::Flag Option>
        struct AugAdd : Op<Option, String<'+', '='>> {
          using Transform = oper::Transform<asdl::Operator::ADD>;
        };
        template <flags::Flag Option>
        struct AugBitAnd : Op<Option, String<'&', '='>> {
          using Transform = oper::Transform<asdl::Operator::BIT_AND>;
        };
        template <flags::Flag Option>
        struct AugBitOr : Op<Option, String<'|', '='>> {
          using Transform = oper::Transform<asdl::Operator::BIT_OR>;
        };
        template <flags::Flag Option>
        struct AugBitXor : Op<Option, String<'^', '='>> {
          using Transform = oper::Transform<asdl::Operator::BIT_XOR>;
        };
        template <flags::Flag Option>
        struct AugDiv : Op<Option, String<'/', '='>> {
          using Transform = oper::Transform<asdl::Operator::DIV>;
        };
        template <flags::Flag Option>
        struct AugFloorDiv : Op<Option, String<'/', '/', '='>> {
          using Transform = oper::Transform<asdl::Operator::FLOOR_DIV>;
        };
        template <flags::Flag Option>
        struct AugLShift : Op<Option, String<'<', '<', '='>> {
          using Transform = oper::Transform<asdl::Operator::L_SHIFT>;
        };
        template <flags::Flag Option>
        struct AugMatMult : Op<Option, String<'@', '='>> {
          using Transform = oper::Transform<asdl::Operator::MAT_MULT>;
        };
        template <flags::Flag Option>
        struct AugMod : Op<Option, String<'%', '='>> {
          using Transform = oper::Transform<asdl::Operator::MOD>;
        };
        template <flags::Flag Option>
        struct AugMult : Op<Option, String<'*', '='>> {
          using Transform = oper::Transform<asdl::Operator::MULT>;
        };
        template <flags::Flag Option>
        struct AugPow : Op<Option, String<'*', '*', '='>> {
          using Transform = oper::Transform<asdl::Operator::POW>;
        };
        template <flags::Flag Option>
        struct AugRShift : Op<Option, String<'>', '>', '='>> {
          using Transform = oper::Transform<asdl::Operator::R_SHIFT>;
        };
        template <flags::Flag Option>
        struct AugSub : Op<Option, String<'-', '='>> {
          using Transform = oper::Transform<asdl::Operator::SUB>;
        };
        template <flags::Flag Option>
        using BitAnd = Op<Option, one<'&'>, '='>;
        template <flags::Flag Option>
        struct BitNot : Op<Option, one<'~'>> {
          using Transform = oper::Transform<asdl::Unary::Op::BIT_NOT>;
        };
        template <flags::Flag Option>
        using BitOr = Op<Option, one<'|'>, '='>;
        template <flags::Flag Option>
        using BitXor = Op<Option, one<'^'>, '='>;
        template <flags::Flag Option>
        using Colon = Op<Option, one<':'>>;
        template <flags::Flag Option>
        using Comma = Op<Option, one<','>>;
        template <flags::Flag Option>
        struct Div : Op<Option, one<'/'>, '=', '/'> {
          using Transform = oper::Transform<asdl::Operator::DIV>;
        };
        template <flags::Flag Option>
        struct Ellipsis : Op<Option, rep<3, one<'.'>>> {
          struct Transform {
            template <typename Outer>
            void success(Outer &&outer) {
              outer.push(asdl::Ellipsis{});
            }
          };
        };
        template <flags::Flag Option>
        using Eq = Op<Option, one<'='>, '='>;
        template <flags::Flag Option>
        struct EqEq : Op<Option, String<'=', '='>> {
          using Transform = oper::Transform<asdl::CompareExpr::Op::EQ>;
        };
        template <flags::Flag Option>
        struct FloorDiv : Op<Option, String<'/', '/'>, '='> {
          using Transform = oper::Transform<asdl::Operator::FLOOR_DIV>;
        };
        template <flags::Flag Option>
        struct Gt : Op<Option, one<'>'>, '=', '>'> {
          using Transform = oper::Transform<asdl::CompareExpr::Op::GT>;
        };
        template <flags::Flag Option>
        struct GtE : Op<Option, String<'>', '='>> {
          using Transform = oper::Transform<asdl::CompareExpr::Op::GT_E>;
        };
        template <flags::Flag Option>
        using LBrc = Op<Option, one<'{'>>;
        template <flags::Flag Option>
        using LBrt = Op<Option, one<'['>>;
        template <flags::Flag Option>
        using LPar = Op<Option, one<'('>>;
        template <flags::Flag Option>
        struct LShift : Op<Option, String<'<', '<'>, '='> {
          using Transform = oper::Transform<asdl::Operator::L_SHIFT>;
        };
        template <flags::Flag Option>
        struct Lt : Op<Option, one<'<'>, '=', '<', '>'> {
          using Transform = oper::Transform<asdl::CompareExpr::Op::LT>;
        };
        template <flags::Flag Option>
        struct LtE : Op<Option, String<'<', '='>> {
          using Transform = oper::Transform<asdl::CompareExpr::Op::LT_E>;
        };
        template <flags::Flag Option>
        struct MatMult : seq<AtImpl<Option>> {
          using Transform = oper::Transform<asdl::Operator::MAT_MULT>;
        };
        template <flags::Flag Option>
        struct Mod : Op<Option, one<'%'>, '='> {
          using Transform = oper::Transform<asdl::Operator::MOD>;
        };
        template <flags::Flag Option>
        using MultImpl = Op<Option, one<'*'>, '*', '='>;
        template <flags::Flag Option>
        struct Mult : seq<MultImpl<Option>> {
          using Transform = oper::Transform<asdl::Operator::MULT>;
        };
        template <flags::Flag Option>
        struct NotEq : Op<Option, sor<String<'!', '='>, String<'<', '>'>>> {
          using Transform = oper::Transform<asdl::CompareExpr::Op::NOT_EQ>;
        };
        template <flags::Flag Option>
        using PeriodImpl = Op<Option, one<'.'>>;
        template <flags::Flag Option>
        using Period = seq<not_at<rep<3, one<'.'>>>, PeriodImpl<Option>>;
        template <flags::Flag Option>
        using PowImpl = Op<Option, String<'*', '*'>, '='>;
        template <flags::Flag Option>
        struct Pow : seq<PowImpl<Option>> {
          using Transform = oper::Transform<asdl::Operator::POW>;
        };
        template <flags::Flag Option>
        using RBrc = Op<Option, one<'}'>>;
        template <flags::Flag Option>
        using RBrt = Op<Option, one<']'>>;
        template <flags::Flag Option>
        using RPar = Op<Option, one<')'>>;
        template <flags::Flag Option>
        struct RShift : Op<Option, String<'>', '>'>, '='> {
          using Transform = oper::Transform<asdl::Operator::R_SHIFT>;
        };
        template <flags::Flag Option>
        using Semi = Op<Option, one<';'>>;
        template <flags::Flag Option>
        using StarOp = seq<MultImpl<Option>>;
        template <flags::Flag Option>
        using SubImpl = Op<Option, one<'-'>, '=', '>'>;
        template <flags::Flag Option>
        struct Sub : seq<SubImpl<Option>> {
          using Transform = oper::Transform<asdl::Operator::SUB>;
        };
        template <flags::Flag Option>
        struct UAdd : Op<Option, one<'+'>, '='> {
          using Transform = oper::Transform<asdl::Unary::ADD>;
        };
        template <flags::Flag Option>
        struct USub : seq<SubImpl<Option>> {
          using Transform = oper::Transform<asdl::Unary::SUB>;
        };
        template <flags::Flag Option>
        using Unpack = seq<PowImpl<Option>>;
      } // namespace token
      using token::Add;
      using token::Arr;
      using token::AtOp;
      using token::AugAdd;
      using token::AugBitAnd;
      using token::AugBitOr;
      using token::AugBitXor;
      using token::AugDiv;
      using token::AugFloorDiv;
      using token::AugLShift;
      using token::AugMatMult;
      using token::AugMod;
      using token::AugMult;
      using token::AugPow;
      using token::AugRShift;
      using token::AugSub;
      using token::BitAnd;
      using token::BitNot;
      using token::BitOr;
      using token::BitXor;
      using token::Colon;
      using token::Comma;
      using token::Div;
      using token::Ellipsis;
      using token::Eq;
      using token::EqEq;
      using token::FloorDiv;
      using token::Gt;
      using token::GtE;
      using token::LBrc;
      using token::LBrt;
      using token::LPar;
      using token::LShift;
      using token::Lt;
      using token::LtE;
      using token::MatMult;
      using token::Mod;
      using token::Mult;
      using token::NotEq;
      using token::Period;
      using token::Pow;
      using token::RBrc;
      using token::RBrt;
      using token::RPar;
      using token::RShift;
      using token::Semi;
      using token::StarOp;
      using token::Sub;
      using token::UAdd;
      using token::Unpack;
      using token::USub;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
