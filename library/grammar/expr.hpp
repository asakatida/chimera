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

//! parse definitions for code expressions.

#pragma once

#include <algorithm>
#include <numeric>
#include <type_traits>

#include <tao/pegtl.hpp>

#include "grammar/control.hpp"
#include "grammar/identifier.hpp"
#include "grammar/keys.hpp"
#include "grammar/number.hpp"
#include "grammar/oper.hpp"
#include "grammar/string.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow,
                template <bool> typename Start,
                template <bool, bool, bool> typename Content,
                template <bool> typename End>
      struct Group : IfMust<Start<true>, Content<true, AsyncFlow, ScopeFlow>,
                            End<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow,
                template <bool> typename Start,
                template <bool, bool, bool> typename Content,
                template <bool> typename End>
      struct GroupOpt
          : IfMust<Start<true>, Opt<Content<true, AsyncFlow, ScopeFlow>>,
                   End<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow,
                template <bool, bool, bool> typename Content>
      using Paren = Group<Implicit, AsyncFlow, ScopeFlow, LPar, Content, RPar>;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow,
                template <bool, bool, bool> typename Content>
      using ParenOpt =
          GroupOpt<Implicit, AsyncFlow, ScopeFlow, LPar, Content, RPar>;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow,
                template <bool, bool, bool> typename Content>
      using Bracket =
          Group<Implicit, AsyncFlow, ScopeFlow, LBrt, Content, RBrt>;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow,
                template <bool, bool, bool> typename Content>
      using BracketOpt =
          GroupOpt<Implicit, AsyncFlow, ScopeFlow, LBrt, Content, RBrt>;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow,
                template <bool, bool, bool> typename Content>
      using Brace = Group<Implicit, AsyncFlow, ScopeFlow, LBrc, Content, RBrc>;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct CompFor;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow,
                template <bool, bool, bool> typename Elt>
      struct CompOrCommaList : Sor<Seq<Elt<Implicit, AsyncFlow, ScopeFlow>,
                                       CompFor<Implicit, AsyncFlow, ScopeFlow>>,
                                   ListTail<Elt<Implicit, AsyncFlow, ScopeFlow>,
                                            Comma<Implicit>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Test;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct VarargsListNameEqTest
          : Seq<Name<Implicit>,
                Opt<Eq<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (!outer.has_value()) {
              outer.push(asdl::Arguments{});
            }
            [this](auto &&arguments) {
              asdl::Arg arg;
              if (this->template top_is<asdl::ExprImpl>()) {
                arg.arg_default = this->template pop<asdl::ExprImpl>();
              }
              arg.name = this->template pop<asdl::Name>();
              (arguments.vararg ? arguments.kwonlyargs : arguments.args)
                  .push_back(std::move(arg));
            }(outer.template top<asdl::Arguments>());
          }
        };
      };
      template <bool Implicit>
      struct VarargsListStarName : Seq<StarOp<Implicit>, Opt<Name<Implicit>>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Arg arg;
            if (has_value()) {
              arg.name = pop<asdl::Name>();
            }
            if (outer.has_value()) {
              outer.template top<asdl::Arguments>().vararg = std::move(arg);
            } else {
              outer.push(asdl::Arguments{{}, std::move(arg), {}, {}});
            }
          }
        };
      };
      template <bool Implicit>
      struct VarargsListUnpackName
          : Seq<Unpack<Implicit>, Name<Implicit>, Opt<Comma<Implicit>>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Arg arg{pop<asdl::Name>(), {}, {}};
            if (outer.has_value()) {
              outer.template top<asdl::Arguments>().kwarg = std::move(arg);
            } else {
              outer.push(asdl::Arguments{{}, {}, {}, std::move(arg)});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct VarargsList
          : Sor<IfMust<
                    VarargsListNameEqTest<Implicit, AsyncFlow, ScopeFlow>,
                    Star<Comma<Implicit>,
                         VarargsListNameEqTest<Implicit, AsyncFlow, ScopeFlow>>,
                    Opt<Comma<Implicit>,
                        Opt<Sor<
                            IfMust<VarargsListStarName<Implicit>,
                                   Star<Comma<Implicit>,
                                        VarargsListNameEqTest<
                                            Implicit, AsyncFlow, ScopeFlow>>,
                                   Opt<Comma<Implicit>,
                                       Opt<VarargsListUnpackName<Implicit>>>>,
                            VarargsListUnpackName<Implicit>>>>>,
                IfMust<
                    VarargsListStarName<Implicit>,
                    Star<Comma<Implicit>,
                         VarargsListNameEqTest<Implicit, AsyncFlow, ScopeFlow>>,
                    Opt<Comma<Implicit>, Opt<VarargsListUnpackName<Implicit>>>>,
                VarargsListUnpackName<Implicit>> {
        struct Transform : rules::Stack<asdl::Arguments> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::Arguments>());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct StarExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestOrStarExpr : Sor<Test<Implicit, AsyncFlow, ScopeFlow>,
                                  StarExpr<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestListStarExpr
          : ListTail<TestOrStarExpr<Implicit, AsyncFlow, ScopeFlow>,
                     Comma<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprList;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct OrTest;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ConditionalExpression
          : Seq<OrTest<Implicit, AsyncFlow, ScopeFlow>,
                Opt<If<Implicit>,
                    Must<OrTest<Implicit, AsyncFlow, ScopeFlow>, Else<Implicit>,
                         Test<Implicit, AsyncFlow, ScopeFlow>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              auto elif = pop<asdl::ExprImpl>();
              auto test = pop<asdl::ExprImpl>();
              auto body = pop<asdl::ExprImpl>();
              outer.push(asdl::ExprImpl{asdl::IfExp{test, body, elif}});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct LambDef : IfMust<Lambda<Implicit>,
                              Opt<VarargsList<Implicit, AsyncFlow, ScopeFlow>>,
                              Colon<Implicit>, Test<Implicit, false, true>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Arguments> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Lambda lambda{{}, pop<asdl::ExprImpl>()};
            if (has_value()) {
              lambda.args = pop<asdl::Arguments>();
            }
            outer.push(asdl::ExprImpl{std::move(lambda)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Test : Sor<ConditionalExpression<Implicit, AsyncFlow, ScopeFlow>,
                        LambDef<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct LambDefNoCond;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestNocond : Sor<OrTest<Implicit, AsyncFlow, ScopeFlow>,
                              LambDefNoCond<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct LambDefNoCond
          : IfMust<Lambda<Implicit>,
                   Opt<VarargsList<Implicit, AsyncFlow, ScopeFlow>>,
                   Colon<Implicit>, TestNocond<Implicit, false, true>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Arguments> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Lambda lambda{{}, pop<asdl::ExprImpl>()};
            if (has_value()) {
              lambda.args = pop<asdl::Arguments>();
            }
            outer.push(asdl::ExprImpl{std::move(lambda)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AndTest;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct OrTest
          : ListMust<AndTest<Implicit, AsyncFlow, ScopeFlow>, Or<Implicit>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bool asdlBool{asdl::Bool::OR, {}};
              asdlBool.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(asdlBool.values));
              outer.push(asdl::ExprImpl{std::move(asdlBool)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct NotTest;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AndTest
          : ListMust<NotTest<Implicit, AsyncFlow, ScopeFlow>, And<Implicit>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bool asdlBool{asdl::Bool::AND, {}};
              asdlBool.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(asdlBool.values));
              outer.push(asdl::ExprImpl{std::move(asdlBool)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Comparison;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct NotTestNot
          : IfMust<Not<Implicit>, NotTest<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{
                asdl::Unary{asdl::Unary::NOT, pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct NotTest : Sor<NotTestNot<Implicit, AsyncFlow, ScopeFlow>,
                           Comparison<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit>
      struct CompOp
          : Sor<Lt<Implicit>, Gt<Implicit>, EqEq<Implicit>, GtE<Implicit>,
                LtE<Implicit>, NotEq<Implicit>, InOp<Implicit>, NotIn<Implicit>,
                Is<Implicit>, IsNot<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ComparisonTail
          : IfMust<CompOp<Implicit>, Expr<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::CompareExpr::Op> {
          template <typename Outer>
          void success(Outer &&outer) {
            [this](auto &&expr) {
              if (!std::holds_alternative<asdl::Compare>(*expr.value)) {
                expr = asdl::Compare{expr, {}};
              }
              std::get<asdl::Compare>(*expr.value)
                  .comparators.push_back(
                      this->template reshape<asdl::CompareExpr,
                                             asdl::CompareExpr::Op,
                                             asdl::ExprImpl>());
            }(outer.template top<asdl::ExprImpl>());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Comparison
          : Seq<Expr<Implicit, AsyncFlow, ScopeFlow>,
                Star<ComparisonTail<Implicit, AsyncFlow, ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct StarExpr
          : Seq<StarOp<Implicit>, Expr<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Starred{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct XorExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Expr
          : ListMust<XorExpr<Implicit, AsyncFlow, ScopeFlow>, BitOr<Implicit>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bin bin{asdl::Operator::BIT_OR, {}};
              bin.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(bin.values));
              outer.push(asdl::ExprImpl{std::move(bin)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AndExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct XorExpr : ListMust<AndExpr<Implicit, AsyncFlow, ScopeFlow>,
                                BitXor<Implicit>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bin bin{asdl::Operator::BIT_XOR, {}};
              bin.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(bin.values));
              outer.push(asdl::ExprImpl{std::move(bin)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ShiftExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AndExpr : ListMust<ShiftExpr<Implicit, AsyncFlow, ScopeFlow>,
                                BitAnd<Implicit>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bin bin{asdl::Operator::BIT_AND, {}};
              bin.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(bin.values));
              outer.push(asdl::ExprImpl{std::move(bin)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArithExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ShiftExprTail : IfMust<Sor<LShift<Implicit>, RShift<Implicit>>,
                                    ArithExpr<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Bin bin{};
            bin.values.reserve(2);
            bin.values = {outer.template pop<asdl::ExprImpl>(),
                          pop<asdl::ExprImpl>()};
            bin.op = pop<asdl::Operator>();
            outer.push(asdl::ExprImpl{std::move(bin)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ShiftExpr
          : Seq<ArithExpr<Implicit, AsyncFlow, ScopeFlow>,
                Star<ShiftExprTail<Implicit, AsyncFlow, ScopeFlow>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Term;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArithExprTail : IfMust<Sor<Add<Implicit>, Sub<Implicit>>,
                                    Term<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Bin bin{};
            bin.values.reserve(2);
            bin.values = {outer.template pop<asdl::ExprImpl>(),
                          pop<asdl::ExprImpl>()};
            bin.op = pop<asdl::Operator>();
            outer.push(asdl::ExprImpl{std::move(bin)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArithExpr
          : Seq<Term<Implicit, AsyncFlow, ScopeFlow>,
                Star<ArithExprTail<Implicit, AsyncFlow, ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Factor;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TermTail
          : IfMust<Sor<Mult<Implicit>, MatMult<Implicit>, Div<Implicit>,
                       Mod<Implicit>, FloorDiv<Implicit>>,
                   Factor<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Bin bin{};
            bin.values.reserve(2);
            bin.values = {outer.template pop<asdl::ExprImpl>(),
                          pop<asdl::ExprImpl>()};
            bin.op = pop<asdl::Operator>();
            outer.push(asdl::ExprImpl{std::move(bin)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Term : Seq<Factor<Implicit, AsyncFlow, ScopeFlow>,
                        Star<TermTail<Implicit, AsyncFlow, ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Power;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Factor
          : Sor<IfMust<Sor<UAdd<Implicit>, USub<Implicit>, BitNot<Implicit>>,
                       Factor<Implicit, AsyncFlow, ScopeFlow>>,
                Power<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Unary::Op> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              asdl::Unary unary{{}, pop<asdl::ExprImpl>()};
              unary.op = pop<asdl::Unary::Op>();
              outer.push(asdl::ExprImpl{std::move(unary)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Power : Seq<AtomExpr<Implicit, AsyncFlow, ScopeFlow>,
                         Opt<Pow<Implicit>,
                             Must<Factor<Implicit, AsyncFlow, ScopeFlow>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              asdl::Bin bin{};
              bin.values.reserve(2);
              bin.values[1] = pop<asdl::ExprImpl>();
              bin.op = pop<asdl::Operator>();
              bin.values[0] = pop<asdl::ExprImpl>();
              outer.push(asdl::ExprImpl{std::move(bin)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Atom;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Trailer;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomExprAtomTrailer
          : Seq<Atom<Implicit, AsyncFlow, ScopeFlow>,
                Star<Trailer<Implicit, AsyncFlow, ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomExprAwait
          : std::conditional_t<
                AsyncFlow,
                IfMust<Await<Implicit>,
                       AtomExprAtomTrailer<Implicit, AsyncFlow, ScopeFlow>>,
                Failure> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Await{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomExpr
          : Sor<AtomExprAwait<Implicit, AsyncFlow, ScopeFlow>,
                AtomExprAtomTrailer<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestListComp;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLParRParTest
          : Seq<Test<Implicit, AsyncFlow, ScopeFlow>, At<RPar<false>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLParRParTestListComp
          : Seq<TestListComp<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (top_is<asdl::Comprehension>()) {
              std::vector<asdl::Comprehension> generators;
              generators.reserve(size() - 1);
              while (size() > 1) {
                generators.push_back(pop<asdl::Comprehension>());
              }
              outer.push(asdl::ExprImpl{asdl::GeneratorExp{
                  pop<asdl::ExprImpl>(), std::move(generators)}});
            } else {
              asdl::Tuple tuple;
              tuple.elts.reserve(size());
              transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
              outer.push(asdl::ExprImpl{std::move(tuple)});
            }
          }
        };
      };
      struct EmptyTuple : Success {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Tuple{}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLParRParContent
          : Sor<YieldExpr<Implicit, AsyncFlow, ScopeFlow>,
                AtomLParRParTest<Implicit, AsyncFlow, ScopeFlow>,
                AtomLParRParTestListComp<Implicit, AsyncFlow, ScopeFlow>,
                EmptyTuple> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLParRPar
          : Paren<Implicit, AsyncFlow, ScopeFlow, AtomLParRParContent> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLBrtRBrt
          : BracketOpt<Implicit, AsyncFlow, ScopeFlow, TestListComp> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (top_is<asdl::Comprehension>()) {
              std::vector<asdl::Comprehension> generators;
              generators.reserve(size() - 1);
              while (size() > 1) {
                generators.push_back(pop<asdl::Comprehension>());
              }
              outer.push(asdl::ExprImpl{asdl::ListComp{pop<asdl::ExprImpl>(),
                                                       std::move(generators)}});
            } else {
              asdl::List list;
              list.elts.reserve(size());
              transform<asdl::ExprImpl>(std::back_inserter(list.elts));
              outer.push(asdl::ExprImpl{std::move(list)});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictMaker;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SetMaker;
      struct EmptyDict : Success {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Dict{}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictOrSetMaker
          : Sor<DictMaker<Implicit, AsyncFlow, ScopeFlow>,
                SetMaker<Implicit, AsyncFlow, ScopeFlow>, EmptyDict> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLBrcRBrc
          : Brace<Implicit, AsyncFlow, ScopeFlow, DictOrSetMaker> {};
      struct AtomNameImpl : Minus<NameImpl, Keywords> {};
      template <bool Implicit>
      struct AtomName : Token<Implicit, AtomNameImpl> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{pop<asdl::Name>()});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Atom : Sor<AtomLParRPar<Implicit, AsyncFlow, ScopeFlow>,
                        AtomLBrtRBrt<Implicit, AsyncFlow, ScopeFlow>,
                        AtomLBrcRBrc<Implicit, AsyncFlow, ScopeFlow>,
                        STRING<Implicit, AsyncFlow, ScopeFlow>,
                        NUMBER<Implicit>, EllipsisOp<Implicit>, None<Implicit>,
                        True<Implicit>, False<Implicit>, AtomName<Implicit>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestListComp
          : CompOrCommaList<Implicit, AsyncFlow, ScopeFlow, TestOrStarExpr> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgList;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TrailerLParRPar
          : ParenOpt<Implicit, AsyncFlow, ScopeFlow, ArgList> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Keyword> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Call call{outer.template pop<asdl::ExprImpl>(), {}, {}};
            call.keywords.reserve(size());
            call.args.reserve(size());
            while (has_value()) {
              if (top_is<asdl::Keyword>()) {
                call.keywords.push_back(pop<asdl::Keyword>());
              } else {
                call.args.push_back(pop<asdl::ExprImpl>());
              }
            }
            std::reverse(call.args.begin(), call.args.end());
            std::reverse(call.keywords.begin(), call.keywords.end());
            outer.push(asdl::ExprImpl{std::move(call)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptList;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TrailerLBrtRBrt
          : Bracket<Implicit, AsyncFlow, ScopeFlow, SubscriptList> {
        struct Transform : rules::Stack<asdl::SliceImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Subscript{
                outer.template pop<asdl::ExprImpl>(), pop<asdl::SliceImpl>()}});
          }
        };
      };
      template <bool Implicit>
      struct TrailerPeriod : IfMust<Period<Implicit>, Name<Implicit>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Attribute{
                outer.template pop<asdl::ExprImpl>(), pop<asdl::Name>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Trailer : Sor<TrailerLParRPar<Implicit, AsyncFlow, ScopeFlow>,
                           TrailerLBrtRBrt<Implicit, AsyncFlow, ScopeFlow>,
                           TrailerPeriod<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Subscript;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptList : ListTail<Subscript<Implicit, AsyncFlow, ScopeFlow>,
                                      Comma<Implicit>> {
        struct Transform : rules::Stack<asdl::SliceImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::ExtSlice extSlice;
              extSlice.dims.reserve(s);
              transform<asdl::SliceImpl>(std::back_inserter(extSlice.dims));
              outer.push(asdl::SliceImpl{std::move(extSlice)});
            } else {
              outer.push(pop<asdl::SliceImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptTest
          : Seq<Test<Implicit, AsyncFlow, ScopeFlow>, NotAt<Colon<false>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::SliceImpl{asdl::Index{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStep : Seq<Colon<Implicit>, Colon<Implicit>,
                                 Test<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(
                asdl::SliceImpl{asdl::Slice{{}, {}, pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStopStep
          : Seq<Colon<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>,
                Colon<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Slice slice{{}, {}, pop<asdl::ExprImpl>()};
            slice.upper = pop<asdl::ExprImpl>();
            outer.push(asdl::SliceImpl{std::move(slice)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStartStep
          : Seq<Test<Implicit, AsyncFlow, ScopeFlow>, Colon<Implicit>,
                Colon<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Slice slice{{}, {}, pop<asdl::ExprImpl>()};
            slice.lower = pop<asdl::ExprImpl>();
            outer.push(asdl::SliceImpl{std::move(slice)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStartStopStep
          : Seq<Test<Implicit, AsyncFlow, ScopeFlow>, Colon<Implicit>,
                Test<Implicit, AsyncFlow, ScopeFlow>, Colon<Implicit>,
                Test<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Slice slice{{}, {}, pop<asdl::ExprImpl>()};
            slice.upper = pop<asdl::ExprImpl>();
            slice.lower = pop<asdl::ExprImpl>();
            outer.push(asdl::SliceImpl{std::move(slice)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStartStop
          : Seq<Test<Implicit, AsyncFlow, ScopeFlow>, Colon<Implicit>,
                Test<Implicit, AsyncFlow, ScopeFlow>, Opt<Colon<Implicit>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Slice slice{{}, pop<asdl::ExprImpl>(), {}};
            slice.lower = pop<asdl::ExprImpl>();
            outer.push(asdl::SliceImpl{std::move(slice)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStop
          : Seq<Colon<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>,
                Opt<Colon<Implicit>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(
                asdl::SliceImpl{asdl::Slice{{}, pop<asdl::ExprImpl>(), {}}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStart : Seq<Opt<Test<Implicit, AsyncFlow, ScopeFlow>>,
                                  Colon<Implicit>, Opt<Colon<Implicit>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (has_value()) {
              outer.push(
                  asdl::SliceImpl{asdl::Slice{pop<asdl::ExprImpl>(), {}, {}}});
            } else {
              outer.push(asdl::SliceImpl{asdl::Slice{}});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Subscript
          : Sor<SubscriptTest<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStep<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStopStep<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStartStep<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStartStopStep<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStop<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStartStop<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStart<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::SliceImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::SliceImpl>());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprList : ListTail<Sor<Expr<Implicit, AsyncFlow, ScopeFlow>,
                                     StarExpr<Implicit, AsyncFlow, ScopeFlow>>,
                                 Comma<Implicit>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 0) {
              asdl::Tuple tuple;
              tuple.elts.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
              outer.push(asdl::ExprImpl{std::move(tuple)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestList
          : ListTail<Test<Implicit, AsyncFlow, ScopeFlow>, Comma<Implicit>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 0) {
              asdl::Tuple tuple;
              tuple.elts.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
              outer.push(asdl::ExprImpl{std::move(tuple)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SetMaker
          : CompOrCommaList<Implicit, AsyncFlow, ScopeFlow, TestOrStarExpr> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (top_is<asdl::Comprehension>()) {
              std::vector<asdl::Comprehension> generators;
              generators.reserve(size() - 1);
              while (size() > 1) {
                generators.push_back(pop<asdl::Comprehension>());
              }
              outer.push(asdl::ExprImpl{
                  asdl::SetComp{pop<asdl::ExprImpl>(), std::move(generators)}});
            } else {
              asdl::Set set;
              set.elts.reserve(size());
              transform<asdl::ExprImpl>(std::back_inserter(set.elts));
              outer.push(asdl::ExprImpl{std::move(set)});
            }
          }
        };
      };
      template <bool Implicit>
      struct DictMakerUnpack : Unpack<Implicit> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::UnpackDict{}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictMakerEltStart
          : Seq<Test<Implicit, AsyncFlow, ScopeFlow>, Colon<Implicit>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::visit(outer, pop());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictMakerElt
          : Sor<IfMust<DictMakerUnpack<Implicit>,
                       Expr<Implicit, AsyncFlow, ScopeFlow>>,
                IfMust<DictMakerEltStart<Implicit, AsyncFlow, ScopeFlow>,
                       Test<Implicit, AsyncFlow, ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictMaker
          : CompOrCommaList<Implicit, AsyncFlow, ScopeFlow, DictMakerElt> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (top_is<asdl::Comprehension>()) {
              std::vector<asdl::Comprehension> generators;
              generators.reserve(size() - 1);
              while (size() > 1) {
                generators.push_back(pop<asdl::Comprehension>());
              }
              auto value = pop<asdl::ExprImpl>();
              outer.push(asdl::ExprImpl{asdl::DictComp{pop<asdl::ExprImpl>(),
                                                       std::move(value),
                                                       std::move(generators)}});
            } else {
              asdl::Dict dict;
              auto s = size() / 2;
              dict.keys.reserve(s);
              dict.values.reserve(s);
              while (has_value()) {
                dict.values.push_back(pop<asdl::ExprImpl>());
                dict.keys.push_back(pop<asdl::ExprImpl>());
              }
              std::reverse(dict.values.begin(), dict.values.end());
              std::reverse(dict.keys.begin(), dict.keys.end());
              outer.push(asdl::ExprImpl{std::move(dict)});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Argument;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgList : ListTail<Argument<Implicit, AsyncFlow, ScopeFlow>,
                                Comma<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgumentArg : Seq<Test<Implicit, AsyncFlow, ScopeFlow>,
                               Opt<CompFor<Implicit, AsyncFlow, ScopeFlow>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              std::vector<asdl::Comprehension> generators;
              generators.reserve(size() - 1);
              while (size() > 1) {
                generators.push_back(pop<asdl::Comprehension>());
              }
              outer.push(asdl::ExprImpl{asdl::GeneratorExp{
                  pop<asdl::ExprImpl>(), std::move(generators)}});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgumentKeyword : Seq<Name<Implicit>, Eq<Implicit>,
                                   Test<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Keyword keyword{{}, pop<asdl::ExprImpl>()};
            keyword.arg = pop<asdl::Name>();
            outer.push(std::move(keyword));
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgumentKeywordUnpack
          : IfMust<Unpack<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Keyword{{}, pop<asdl::ExprImpl>()});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgumentArgUnpack
          : IfMust<StarOp<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Starred{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Argument
          : Sor<ArgumentKeywordUnpack<Implicit, AsyncFlow, ScopeFlow>,
                ArgumentArgUnpack<Implicit, AsyncFlow, ScopeFlow>,
                ArgumentKeyword<Implicit, AsyncFlow, ScopeFlow>,
                ArgumentArg<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Keyword> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::visit(outer, pop());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct CompIf
          : IfMust<If<Implicit>, TestNocond<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.template top<asdl::Comprehension>().ifs.push_back(
                pop<asdl::ExprImpl>());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct CompForAsync
          : std::conditional_t<
                AsyncFlow,
                IfMust<Async<Implicit>, For<Implicit>,
                       ExprList<Implicit, AsyncFlow, ScopeFlow>, In<Implicit>,
                       OrTest<Implicit, AsyncFlow, ScopeFlow>>,
                Failure> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            auto iter = pop<asdl::ExprImpl>();
            auto target = pop<asdl::ExprImpl>();
            outer.push(asdl::Comprehension{target, iter, {}, true});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct CompForSync
          : IfMust<For<Implicit>, ExprList<Implicit, AsyncFlow, ScopeFlow>,
                   In<Implicit>, OrTest<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            auto iter = pop<asdl::ExprImpl>();
            auto target = pop<asdl::ExprImpl>();
            outer.push(asdl::Comprehension{target, iter, {}, false});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct CompFor
          : Seq<Sor<CompForAsync<Implicit, AsyncFlow, ScopeFlow>,
                    CompForSync<Implicit, AsyncFlow, ScopeFlow>>,
                Star<Sor<CompForAsync<Implicit, AsyncFlow, ScopeFlow>,
                         CompForSync<Implicit, AsyncFlow, ScopeFlow>,
                         CompIf<Implicit, AsyncFlow, ScopeFlow>>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldArg;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldExpr : std::conditional_t<
                             ScopeFlow || AsyncFlow,
                             Seq<Yield<Implicit>,
                                 Opt<YieldArg<Implicit, AsyncFlow, ScopeFlow>>>,
                             Failure> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (has_value()) {
              outer.push(pop<asdl::ExprImpl>());
            } else {
              outer.push(asdl::ExprImpl{asdl::Yield{}});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldArgFrom
          : IfMust<From<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::YieldFrom{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldArgTestList : Seq<TestList<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Yield{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldArg : Sor<YieldArgFrom<Implicit, AsyncFlow, ScopeFlow>,
                            YieldArgTestList<Implicit, AsyncFlow, ScopeFlow>> {
      };
    } // namespace grammar
  }   // namespace library
} // namespace chimera
