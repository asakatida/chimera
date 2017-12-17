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

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/rep_one_min_max.hpp>

#include "asdl/asdl.hpp"
#include "grammar/control.hpp"
#include "grammar/whitespace.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <bool Implicit, typename String, char... Cs>
      using Op =
          Token<Implicit,
                std::conditional_t<
                    (sizeof...(Cs) > 0),
                    tao::pegtl::seq<String,
                                    tao::pegtl::not_at<tao::pegtl::one<Cs...>>>,
                    String>>;
      template <bool Implicit, char One, char... Cs>
      using OpSimple =
          Token<Implicit,
                std::conditional_t<
                    (sizeof...(Cs) > 0),
                    tao::pegtl::seq<tao::pegtl::one<One>,
                                    tao::pegtl::not_at<tao::pegtl::one<Cs...>>>,
                    tao::pegtl::one<One>>>;
      template <bool Implicit>
      struct Add : OpSimple<Implicit, '+', '='> {};
      template <bool Implicit>
      struct Actions<Add<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::ADD);
        }
      };
      template <bool Implicit>
      struct Arr : Op<Implicit, tao::pegtl::string<'-', '>'>> {};
      template <bool Implicit>
      struct AtImpl : OpSimple<Implicit, '@', '='> {};
      template <bool Implicit>
      struct At : tao::pegtl::seq<AtImpl<Implicit>> {};
      template <bool Implicit>
      struct AugAdd : Op<Implicit, tao::pegtl::string<'+', '='>> {};
      template <bool Implicit>
      struct Actions<AugAdd<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::ADD);
        }
      };
      template <bool Implicit>
      struct AugBitAnd : Op<Implicit, tao::pegtl::string<'&', '='>> {};
      template <bool Implicit>
      struct Actions<AugBitAnd<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::BIT_AND);
        }
      };
      template <bool Implicit>
      struct AugBitOr : Op<Implicit, tao::pegtl::string<'|', '='>> {};
      template <bool Implicit>
      struct Actions<AugBitOr<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::BIT_OR);
        }
      };
      template <bool Implicit>
      struct AugBitXor : Op<Implicit, tao::pegtl::string<'^', '='>> {};
      template <bool Implicit>
      struct Actions<AugBitXor<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::BIT_XOR);
        }
      };
      template <bool Implicit>
      struct AugDiv : Op<Implicit, tao::pegtl::string<'/', '='>> {};
      template <bool Implicit>
      struct Actions<AugDiv<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::DIV);
        }
      };
      template <bool Implicit>
      struct AugFloorDiv : Op<Implicit, tao::pegtl::string<'/', '/', '='>> {};
      template <bool Implicit>
      struct Actions<AugFloorDiv<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::FLOOR_DIV);
        }
      };
      template <bool Implicit>
      struct AugLShift : Op<Implicit, tao::pegtl::string<'<', '<', '='>> {};
      template <bool Implicit>
      struct Actions<AugLShift<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::L_SHIFT);
        }
      };
      template <bool Implicit>
      struct AugMatMult : Op<Implicit, tao::pegtl::string<'@', '='>> {};
      template <bool Implicit>
      struct Actions<AugMatMult<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MAT_MULT);
        }
      };
      template <bool Implicit>
      struct AugMod : Op<Implicit, tao::pegtl::string<'%', '='>> {};
      template <bool Implicit>
      struct Actions<AugMod<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MOD);
        }
      };
      template <bool Implicit>
      struct AugMult : Op<Implicit, tao::pegtl::string<'*', '='>> {};
      template <bool Implicit>
      struct Actions<AugMult<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MULT);
        }
      };
      template <bool Implicit>
      struct AugPow : Op<Implicit, tao::pegtl::string<'*', '*', '='>> {};
      template <bool Implicit>
      struct Actions<AugPow<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::POW);
        }
      };
      template <bool Implicit>
      struct AugRShift : Op<Implicit, tao::pegtl::string<'>', '>', '='>> {};
      template <bool Implicit>
      struct Actions<AugRShift<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::R_SHIFT);
        }
      };
      template <bool Implicit>
      struct AugSub : Op<Implicit, tao::pegtl::string<'-', '='>> {};
      template <bool Implicit>
      struct Actions<AugSub<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::SUB);
        }
      };
      template <bool Implicit>
      struct BitAnd : OpSimple<Implicit, '&', '='> {};
      template <bool Implicit>
      struct BitNot : OpSimple<Implicit, '~'> {};
      template <bool Implicit>
      struct Actions<BitNot<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Unary::BIT_NOT);
        }
      };
      template <bool Implicit>
      struct BitOr : OpSimple<Implicit, '|', '='> {};
      template <bool Implicit>
      struct BitXor : OpSimple<Implicit, '^', '='> {};
      template <bool Implicit>
      struct Colon : OpSimple<Implicit, ':'> {};
      template <bool Implicit>
      struct Comma : OpSimple<Implicit, ','> {};
      template <bool Implicit>
      struct Div : OpSimple<Implicit, '/', '=', '/'> {};
      template <bool Implicit>
      struct Actions<Div<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::DIV);
        }
      };
      template <bool Implicit>
      struct Ellipsis : Op<Implicit, tao::pegtl::ellipsis> {};
      template <bool Implicit>
      struct Actions<Ellipsis<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::ExprImpl{asdl::Ellipsis{}});
        }
      };
      template <bool Implicit>
      struct Eq : OpSimple<Implicit, '=', '='> {};
      template <bool Implicit>
      struct EqEq : Op<Implicit, tao::pegtl::two<'='>> {};
      template <bool Implicit>
      struct Actions<EqEq<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::EQ);
        }
      };
      template <bool Implicit>
      struct FloorDiv : Op<Implicit, tao::pegtl::two<'/'>, '='> {};
      template <bool Implicit>
      struct Actions<FloorDiv<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::FLOOR_DIV);
        }
      };
      template <bool Implicit>
      struct Gt : OpSimple<Implicit, '>', '=', '>'> {};
      template <bool Implicit>
      struct Actions<Gt<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::GT);
        }
      };
      template <bool Implicit>
      struct GtE : Op<Implicit, tao::pegtl::string<'>', '='>> {};
      template <bool Implicit>
      struct Actions<GtE<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::GT_E);
        }
      };
      template <bool Implicit>
      struct LBrc : OpSimple<Implicit, '{'> {};
      template <bool Implicit>
      struct LBrt : OpSimple<Implicit, '['> {};
      template <bool Implicit>
      struct LPar : OpSimple<Implicit, '('> {};
      template <bool Implicit>
      struct LShift : Op<Implicit, tao::pegtl::two<'<'>, '='> {};
      template <bool Implicit>
      struct Actions<LShift<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::L_SHIFT);
        }
      };
      template <bool Implicit>
      struct Lt : OpSimple<Implicit, '<', '=', '<', '>'> {};
      template <bool Implicit>
      struct Actions<Lt<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::LT);
        }
      };
      template <bool Implicit>
      struct LtE : Op<Implicit, tao::pegtl::string<'<', '='>> {};
      template <bool Implicit>
      struct Actions<LtE<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::LT_E);
        }
      };
      template <bool Implicit>
      struct MatMult : tao::pegtl::seq<AtImpl<Implicit>> {};
      template <bool Implicit>
      struct Actions<MatMult<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MAT_MULT);
        }
      };
      template <bool Implicit>
      struct Mod : OpSimple<Implicit, '%', '='> {};
      template <bool Implicit>
      struct Actions<Mod<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MOD);
        }
      };
      template <bool Implicit>
      struct MultImpl : OpSimple<Implicit, '*', '*', '='> {};
      template <bool Implicit>
      struct Mult : tao::pegtl::seq<MultImpl<Implicit>> {};
      template <bool Implicit>
      struct Actions<Mult<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::MULT);
        }
      };
      template <bool Implicit>
      struct NotEq
          : Op<Implicit, tao::pegtl::sor<tao::pegtl::string<'!', '='>,
                                         tao::pegtl::string<'<', '>'>>> {};
      template <bool Implicit>
      struct Actions<NotEq<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::NOT_EQ);
        }
      };
      template <bool Implicit>
      struct PeriodImpl : OpSimple<Implicit, '.'> {};
      template <bool Implicit>
      struct Period : tao::pegtl::seq<tao::pegtl::not_at<tao::pegtl::ellipsis>,
                                      PeriodImpl<Implicit>> {};
      template <bool Implicit>
      struct PowImpl : Op<Implicit, tao::pegtl::two<'*'>, '='> {};
      template <bool Implicit>
      struct Pow : tao::pegtl::seq<PowImpl<Implicit>> {};
      template <bool Implicit>
      struct Actions<Pow<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::POW);
        }
      };
      template <bool Implicit>
      struct RBrc : OpSimple<Implicit, '}'> {};
      template <bool Implicit>
      struct RBrt : OpSimple<Implicit, ']'> {};
      template <bool Implicit>
      struct RPar : OpSimple<Implicit, ')'> {};
      template <bool Implicit>
      struct RShift : Op<Implicit, tao::pegtl::two<'>'>, '='> {};
      template <bool Implicit>
      struct Actions<RShift<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::R_SHIFT);
        }
      };
      template <bool Implicit>
      struct Semi : OpSimple<Implicit, ';'> {};
      template <bool Implicit>
      struct Star : tao::pegtl::seq<MultImpl<Implicit>> {};
      template <bool Implicit>
      struct SubImpl : OpSimple<Implicit, '-', '=', '>'> {};
      template <bool Implicit>
      struct Sub : tao::pegtl::seq<SubImpl<Implicit>> {};
      template <bool Implicit>
      struct Actions<Sub<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Operator::SUB);
        }
      };
      template <bool Implicit>
      struct UAdd : OpSimple<Implicit, '+', '='> {};
      template <bool Implicit>
      struct Actions<UAdd<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Unary::ADD);
        }
      };
      template <bool Implicit>
      struct USub : tao::pegtl::seq<SubImpl<Implicit>> {};
      template <bool Implicit>
      struct Actions<USub<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::Unary::SUB);
        }
      };
      template <bool Implicit>
      struct Unpack : tao::pegtl::seq<PowImpl<Implicit>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
