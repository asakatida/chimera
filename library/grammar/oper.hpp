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

//! parse definitions for operator tokens.

#pragma once

#include <type_traits>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/rep_one_min_max.hpp>

#include "asdl/asdl.hpp"
#include "grammar/control.hpp"
#include "grammar/whitespace.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <bool Implicit, typename Rule, char... Cs>
      using Op = Token<
          Implicit, Rule,
          std::conditional_t<(sizeof...(Cs) > 0), NotAt<One<Cs...>>, Success>>;
      template <bool Implicit>
      struct Add : Op<Implicit, One<'+'>, '='> {};
      template <bool Implicit>
      struct ChimeraActions<Add<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::ADD);
        }
      };
      template <bool Implicit>
      struct Arr : Op<Implicit, String<'-', '>'>> {};
      template <bool Implicit>
      struct AtImpl : Op<Implicit, One<'@'>, '='> {};
      template <bool Implicit>
      struct AtOp : Seq<AtImpl<Implicit>> {};
      template <bool Implicit>
      struct AugAdd : Op<Implicit, String<'+', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugAdd<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::ADD);
        }
      };
      template <bool Implicit>
      struct AugBitAnd : Op<Implicit, String<'&', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugBitAnd<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::BIT_AND);
        }
      };
      template <bool Implicit>
      struct AugBitOr : Op<Implicit, String<'|', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugBitOr<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::BIT_OR);
        }
      };
      template <bool Implicit>
      struct AugBitXor : Op<Implicit, String<'^', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugBitXor<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::BIT_XOR);
        }
      };
      template <bool Implicit>
      struct AugDiv : Op<Implicit, String<'/', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugDiv<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::DIV);
        }
      };
      template <bool Implicit>
      struct AugFloorDiv : Op<Implicit, String<'/', '/', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugFloorDiv<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::FLOOR_DIV);
        }
      };
      template <bool Implicit>
      struct AugLShift : Op<Implicit, String<'<', '<', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugLShift<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::L_SHIFT);
        }
      };
      template <bool Implicit>
      struct AugMatMult : Op<Implicit, String<'@', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugMatMult<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MAT_MULT);
        }
      };
      template <bool Implicit>
      struct AugMod : Op<Implicit, String<'%', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugMod<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MOD);
        }
      };
      template <bool Implicit>
      struct AugMult : Op<Implicit, String<'*', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugMult<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MULT);
        }
      };
      template <bool Implicit>
      struct AugPow : Op<Implicit, String<'*', '*', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugPow<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::POW);
        }
      };
      template <bool Implicit>
      struct AugRShift : Op<Implicit, String<'>', '>', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugRShift<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::R_SHIFT);
        }
      };
      template <bool Implicit>
      struct AugSub : Op<Implicit, String<'-', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<AugSub<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::SUB);
        }
      };
      template <bool Implicit>
      struct BitAnd : Op<Implicit, One<'&'>, '='> {};
      template <bool Implicit>
      struct BitNot : Op<Implicit, One<'~'>> {};
      template <bool Implicit>
      struct ChimeraActions<BitNot<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Unary::BIT_NOT);
        }
      };
      template <bool Implicit>
      struct BitOr : Op<Implicit, One<'|'>, '='> {};
      template <bool Implicit>
      struct BitXor : Op<Implicit, One<'^'>, '='> {};
      template <bool Implicit>
      struct Colon : Op<Implicit, One<':'>> {};
      template <bool Implicit>
      struct Comma : Op<Implicit, One<','>> {};
      template <bool Implicit>
      struct Div : Op<Implicit, One<'/'>, '=', '/'> {};
      template <bool Implicit>
      struct ChimeraActions<Div<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::DIV);
        }
      };
      template <bool Implicit>
      struct EllipsisOp : Op<Implicit, Ellipsis> {};
      template <bool Implicit>
      struct ChimeraActions<EllipsisOp<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::ExprImpl{asdl::Ellipsis{}});
        }
      };
      template <bool Implicit>
      struct Eq : Op<Implicit, One<'='>, '='> {};
      template <bool Implicit>
      struct EqEq : Op<Implicit, String<'=', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<EqEq<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::EQ);
        }
      };
      template <bool Implicit>
      struct FloorDiv : Op<Implicit, String<'/', '/'>, '='> {};
      template <bool Implicit>
      struct ChimeraActions<FloorDiv<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::FLOOR_DIV);
        }
      };
      template <bool Implicit>
      struct Gt : Op<Implicit, One<'>'>, '=', '>'> {};
      template <bool Implicit>
      struct ChimeraActions<Gt<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::GT);
        }
      };
      template <bool Implicit>
      struct GtE : Op<Implicit, String<'>', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<GtE<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::GT_E);
        }
      };
      template <bool Implicit>
      struct LBrc : Op<Implicit, One<'{'>> {};
      template <bool Implicit>
      struct LBrt : Op<Implicit, One<'['>> {};
      template <bool Implicit>
      struct LPar : Op<Implicit, One<'('>> {};
      template <bool Implicit>
      struct LShift : Op<Implicit, String<'<', '<'>, '='> {};
      template <bool Implicit>
      struct ChimeraActions<LShift<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::L_SHIFT);
        }
      };
      template <bool Implicit>
      struct Lt : Op<Implicit, One<'<'>, '=', '<', '>'> {};
      template <bool Implicit>
      struct ChimeraActions<Lt<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::LT);
        }
      };
      template <bool Implicit>
      struct LtE : Op<Implicit, String<'<', '='>> {};
      template <bool Implicit>
      struct ChimeraActions<LtE<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::LT_E);
        }
      };
      template <bool Implicit>
      struct MatMult : Seq<AtImpl<Implicit>> {};
      template <bool Implicit>
      struct ChimeraActions<MatMult<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MAT_MULT);
        }
      };
      template <bool Implicit>
      struct Mod : Op<Implicit, One<'%'>, '='> {};
      template <bool Implicit>
      struct ChimeraActions<Mod<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MOD);
        }
      };
      template <bool Implicit>
      struct MultImpl : Op<Implicit, One<'*'>, '*', '='> {};
      template <bool Implicit>
      struct Mult : Seq<MultImpl<Implicit>> {};
      template <bool Implicit>
      struct ChimeraActions<Mult<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MULT);
        }
      };
      template <bool Implicit>
      struct NotEq : Op<Implicit, Sor<String<'!', '='>, String<'<', '>'>>> {};
      template <bool Implicit>
      struct ChimeraActions<NotEq<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::NOT_EQ);
        }
      };
      template <bool Implicit>
      struct PeriodImpl : Op<Implicit, One<'.'>> {};
      template <bool Implicit>
      struct Period : Seq<NotAt<Ellipsis>, PeriodImpl<Implicit>> {};
      template <bool Implicit>
      struct PowImpl : Op<Implicit, String<'*', '*'>, '='> {};
      template <bool Implicit>
      struct Pow : Seq<PowImpl<Implicit>> {};
      template <bool Implicit>
      struct ChimeraActions<Pow<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::POW);
        }
      };
      template <bool Implicit>
      struct RBrc : Op<Implicit, One<'}'>> {};
      template <bool Implicit>
      struct RBrt : Op<Implicit, One<']'>> {};
      template <bool Implicit>
      struct RPar : Op<Implicit, One<')'>> {};
      template <bool Implicit>
      struct RShift : Op<Implicit, String<'>', '>'>, '='> {};
      template <bool Implicit>
      struct ChimeraActions<RShift<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::R_SHIFT);
        }
      };
      template <bool Implicit>
      struct Semi : Op<Implicit, One<';'>> {};
      template <bool Implicit>
      struct StarOp : Seq<MultImpl<Implicit>> {};
      template <bool Implicit>
      struct SubImpl : Op<Implicit, One<'-'>, '=', '>'> {};
      template <bool Implicit>
      struct Sub : Seq<SubImpl<Implicit>> {};
      template <bool Implicit>
      struct ChimeraActions<Sub<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::SUB);
        }
      };
      template <bool Implicit>
      struct UAdd : Op<Implicit, One<'+'>, '='> {};
      template <bool Implicit>
      struct ChimeraActions<UAdd<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Unary::ADD);
        }
      };
      template <bool Implicit>
      struct USub : Seq<SubImpl<Implicit>> {};
      template <bool Implicit>
      struct ChimeraActions<USub<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Unary::SUB);
        }
      };
      template <bool Implicit>
      struct Unpack : Seq<PowImpl<Implicit>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
