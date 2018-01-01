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
      namespace op {
        template <auto Asdl>
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(Asdl);
          }
        };
        template <typename Option, typename Rule, char... Cs>
        using Op = Token<Option, Rule,
                         std::conditional_t<(sizeof...(Cs) > 0),
                                            NotAt<One<Cs...>>, Success>>;
        template <typename Option>
        struct Add : Op<Option, One<'+'>, '='> {
          using Transform = Transform<asdl::Operator::ADD>;
        };
        template <typename Option>
        struct Arr : Op<Option, String<'-', '>'>> {};
        template <typename Option>
        struct AtImpl : Op<Option, One<'@'>, '='> {};
        template <typename Option>
        struct AtOp : Seq<AtImpl<Option>> {};
        template <typename Option>
        struct AugAdd : Op<Option, String<'+', '='>> {
          using Transform = Transform<asdl::Operator::ADD>;
        };
        template <typename Option>
        struct AugBitAnd : Op<Option, String<'&', '='>> {
          using Transform = Transform<asdl::Operator::BIT_AND>;
        };
        template <typename Option>
        struct AugBitOr : Op<Option, String<'|', '='>> {
          using Transform = Transform<asdl::Operator::BIT_OR>;
        };
        template <typename Option>
        struct AugBitXor : Op<Option, String<'^', '='>> {
          using Transform = Transform<asdl::Operator::BIT_XOR>;
        };
        template <typename Option>
        struct AugDiv : Op<Option, String<'/', '='>> {
          using Transform = Transform<asdl::Operator::DIV>;
        };
        template <typename Option>
        struct AugFloorDiv : Op<Option, String<'/', '/', '='>> {
          using Transform = Transform<asdl::Operator::FLOOR_DIV>;
        };
        template <typename Option>
        struct AugLShift : Op<Option, String<'<', '<', '='>> {
          using Transform = Transform<asdl::Operator::L_SHIFT>;
        };
        template <typename Option>
        struct AugMatMult : Op<Option, String<'@', '='>> {
          using Transform = Transform<asdl::Operator::MAT_MULT>;
        };
        template <typename Option>
        struct AugMod : Op<Option, String<'%', '='>> {
          using Transform = Transform<asdl::Operator::MOD>;
        };
        template <typename Option>
        struct AugMult : Op<Option, String<'*', '='>> {
          using Transform = Transform<asdl::Operator::MULT>;
        };
        template <typename Option>
        struct AugPow : Op<Option, String<'*', '*', '='>> {
          using Transform = Transform<asdl::Operator::POW>;
        };
        template <typename Option>
        struct AugRShift : Op<Option, String<'>', '>', '='>> {
          using Transform = Transform<asdl::Operator::R_SHIFT>;
        };
        template <typename Option>
        struct AugSub : Op<Option, String<'-', '='>> {
          using Transform = Transform<asdl::Operator::SUB>;
        };
        template <typename Option>
        struct BitAnd : Op<Option, One<'&'>, '='> {};
        template <typename Option>
        struct BitNot : Op<Option, One<'~'>> {
          using Transform = Transform<asdl::Unary::Op::BIT_NOT>;
        };
        template <typename Option>
        struct BitOr : Op<Option, One<'|'>, '='> {};
        template <typename Option>
        struct BitXor : Op<Option, One<'^'>, '='> {};
        template <typename Option>
        struct Colon : Op<Option, One<':'>> {};
        template <typename Option>
        struct Comma : Op<Option, One<','>> {};
        template <typename Option>
        struct Div : Op<Option, One<'/'>, '=', '/'> {
          using Transform = Transform<asdl::Operator::DIV>;
        };
        template <typename Option>
        struct EllipsisOp : Op<Option, Ellipsis> {
          struct Transform {
            template <typename Outer>
            void success(Outer &&outer) {
              outer.push(asdl::ExprImpl{asdl::Ellipsis{}});
            }
          };
        };
        template <typename Option>
        struct Eq : Op<Option, One<'='>, '='> {};
        template <typename Option>
        struct EqEq : Op<Option, String<'=', '='>> {
          using Transform = Transform<asdl::CompareExpr::Op::EQ>;
        };
        template <typename Option>
        struct FloorDiv : Op<Option, String<'/', '/'>, '='> {
          using Transform = Transform<asdl::Operator::FLOOR_DIV>;
        };
        template <typename Option>
        struct Gt : Op<Option, One<'>'>, '=', '>'> {
          using Transform = Transform<asdl::CompareExpr::Op::GT>;
        };
        template <typename Option>
        struct GtE : Op<Option, String<'>', '='>> {
          using Transform = Transform<asdl::CompareExpr::Op::GT_E>;
        };
        template <typename Option>
        struct LBrc : Op<Option, One<'{'>> {};
        template <typename Option>
        struct LBrt : Op<Option, One<'['>> {};
        template <typename Option>
        struct LPar : Op<Option, One<'('>> {};
        template <typename Option>
        struct LShift : Op<Option, String<'<', '<'>, '='> {
          using Transform = Transform<asdl::Operator::L_SHIFT>;
        };
        template <typename Option>
        struct Lt : Op<Option, One<'<'>, '=', '<', '>'> {
          using Transform = Transform<asdl::CompareExpr::Op::LT>;
        };
        template <typename Option>
        struct LtE : Op<Option, String<'<', '='>> {
          using Transform = Transform<asdl::CompareExpr::Op::LT_E>;
        };
        template <typename Option>
        struct MatMult : Seq<AtImpl<Option>> {
          using Transform = Transform<asdl::Operator::MAT_MULT>;
        };
        template <typename Option>
        struct Mod : Op<Option, One<'%'>, '='> {
          using Transform = Transform<asdl::Operator::MOD>;
        };
        template <typename Option>
        struct MultImpl : Op<Option, One<'*'>, '*', '='> {};
        template <typename Option>
        struct Mult : Seq<MultImpl<Option>> {
          using Transform = Transform<asdl::Operator::MULT>;
        };
        template <typename Option>
        struct NotEq : Op<Option, Sor<String<'!', '='>, String<'<', '>'>>> {
          using Transform = Transform<asdl::CompareExpr::Op::NOT_EQ>;
        };
        template <typename Option>
        struct PeriodImpl : Op<Option, One<'.'>> {};
        template <typename Option>
        struct Period : Seq<NotAt<Ellipsis>, PeriodImpl<Option>> {};
        template <typename Option>
        struct PowImpl : Op<Option, String<'*', '*'>, '='> {};
        template <typename Option>
        struct Pow : Seq<PowImpl<Option>> {
          using Transform = Transform<asdl::Operator::POW>;
        };
        template <typename Option>
        struct RBrc : Op<Option, One<'}'>> {};
        template <typename Option>
        struct RBrt : Op<Option, One<']'>> {};
        template <typename Option>
        struct RPar : Op<Option, One<')'>> {};
        template <typename Option>
        struct RShift : Op<Option, String<'>', '>'>, '='> {
          using Transform = Transform<asdl::Operator::R_SHIFT>;
        };
        template <typename Option>
        struct Semi : Op<Option, One<';'>> {};
        template <typename Option>
        struct StarOp : Seq<MultImpl<Option>> {};
        template <typename Option>
        struct SubImpl : Op<Option, One<'-'>, '=', '>'> {};
        template <typename Option>
        struct Sub : Seq<SubImpl<Option>> {
          using Transform = Transform<asdl::Operator::SUB>;
        };
        template <typename Option>
        struct UAdd : Op<Option, One<'+'>, '='> {
          using Transform = Transform<asdl::Unary::ADD>;
        };
        template <typename Option>
        struct USub : Seq<SubImpl<Option>> {
          using Transform = Transform<asdl::Unary::SUB>;
        };
        template <typename Option>
        struct Unpack : Seq<PowImpl<Option>> {};
      } // namespace op
      using op::Add;
      using op::Arr;
      using op::AtOp;
      using op::AugAdd;
      using op::AugBitAnd;
      using op::AugBitOr;
      using op::AugBitXor;
      using op::AugDiv;
      using op::AugFloorDiv;
      using op::AugLShift;
      using op::AugMatMult;
      using op::AugMod;
      using op::AugMult;
      using op::AugPow;
      using op::AugRShift;
      using op::AugSub;
      using op::BitAnd;
      using op::BitNot;
      using op::BitOr;
      using op::BitXor;
      using op::Colon;
      using op::Comma;
      using op::Div;
      using op::EllipsisOp;
      using op::Eq;
      using op::EqEq;
      using op::FloorDiv;
      using op::Gt;
      using op::GtE;
      using op::LBrc;
      using op::LBrt;
      using op::LPar;
      using op::LShift;
      using op::Lt;
      using op::LtE;
      using op::MatMult;
      using op::Mod;
      using op::Mult;
      using op::NotEq;
      using op::Period;
      using op::Pow;
      using op::RBrc;
      using op::RBrt;
      using op::RPar;
      using op::RShift;
      using op::Semi;
      using op::StarOp;
      using op::Sub;
      using op::UAdd;
      using op::USub;
      using op::Unpack;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
