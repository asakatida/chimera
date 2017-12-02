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
      struct Group : tao::pegtl::if_must<Start<true>,
                                         Content<true, AsyncFlow, ScopeFlow>,
                                         End<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow,
                template <bool> typename Start,
                template <bool, bool, bool> typename Content,
                template <bool> typename End>
      struct GroupOpt
          : tao::pegtl::if_must<
                Start<true>,
                tao::pegtl::opt<Content<true, AsyncFlow, ScopeFlow>>,
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
      struct CompOrCommaList
          : tao::pegtl::sor<
                tao::pegtl::seq<Elt<Implicit, AsyncFlow, ScopeFlow>,
                                CompFor<Implicit, AsyncFlow, ScopeFlow>>,
                tao::pegtl::list_tail<Elt<Implicit, AsyncFlow, ScopeFlow>,
                                      Comma<Implicit>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Test;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct VarargsListNameEqTest
          : tao::pegtl::seq<
                Name<Implicit>,
                tao::pegtl::opt<Eq<Implicit>,
                                Test<Implicit, AsyncFlow, ScopeFlow>>> {};
      struct VarargsListNameEqTestAction : Stack<asdl::ExprImpl, asdl::Name> {
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
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<VarargsListNameEqTest<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<VarargsListNameEqTest<Implicit, AsyncFlow, ScopeFlow>,
                        VarargsListNameEqTestAction> {};
      template <bool Implicit>
      struct VarargsListStarName
          : tao::pegtl::seq<Star<Implicit>, tao::pegtl::opt<Name<Implicit>>> {};
      struct VarargsListStarNameAction : Stack<asdl::Name> {
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
      template <bool Implicit>
      struct Control<VarargsListStarName<Implicit>>
          : ChangeState<VarargsListStarName<Implicit>,
                        VarargsListStarNameAction> {};
      template <bool Implicit>
      struct VarargsListUnpackName
          : tao::pegtl::seq<Unpack<Implicit>, Name<Implicit>,
                            tao::pegtl::opt<Comma<Implicit>>> {};
      struct VarargsListUnpackNameAction : Stack<asdl::Name> {
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
      template <bool Implicit>
      struct Control<VarargsListUnpackName<Implicit>>
          : ChangeState<VarargsListUnpackName<Implicit>,
                        VarargsListUnpackNameAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct VarargsList
          : tao::pegtl::sor<
                tao::pegtl::if_must<
                    VarargsListNameEqTest<Implicit, AsyncFlow, ScopeFlow>,
                    tao::pegtl::star<
                        Comma<Implicit>,
                        VarargsListNameEqTest<Implicit, AsyncFlow, ScopeFlow>>,
                    tao::pegtl::opt<
                        Comma<Implicit>,
                        tao::pegtl::opt<tao::pegtl::sor<
                            tao::pegtl::if_must<
                                VarargsListStarName<Implicit>,
                                tao::pegtl::star<
                                    Comma<Implicit>,
                                    VarargsListNameEqTest<Implicit, AsyncFlow,
                                                          ScopeFlow>>,
                                tao::pegtl::opt<
                                    Comma<Implicit>,
                                    tao::pegtl::opt<
                                        VarargsListUnpackName<Implicit>>>>,
                            VarargsListUnpackName<Implicit>>>>>,
                tao::pegtl::if_must<
                    VarargsListStarName<Implicit>,
                    tao::pegtl::star<
                        Comma<Implicit>,
                        VarargsListNameEqTest<Implicit, AsyncFlow, ScopeFlow>>,
                    tao::pegtl::opt<
                        Comma<Implicit>,
                        tao::pegtl::opt<VarargsListUnpackName<Implicit>>>>,
                VarargsListUnpackName<Implicit>> {};
      struct VarargsListAction : Stack<asdl::Arguments> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(pop<asdl::Arguments>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<VarargsList<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<VarargsList<Implicit, AsyncFlow, ScopeFlow>,
                        VarargsListAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct StarExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestOrStarExpr
          : tao::pegtl::sor<Test<Implicit, AsyncFlow, ScopeFlow>,
                            StarExpr<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestListStarExpr
          : tao::pegtl::list_tail<
                TestOrStarExpr<Implicit, AsyncFlow, ScopeFlow>,
                Comma<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprList;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct OrTest;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ConditionalExpression
          : tao::pegtl::seq<
                OrTest<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::opt<
                    If<Implicit>,
                    tao::pegtl::must<OrTest<Implicit, AsyncFlow, ScopeFlow>,
                                     Else<Implicit>,
                                     Test<Implicit, AsyncFlow, ScopeFlow>>>> {};
      struct ConditionalExpressionAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (size() > 1) {
            asdl::IfExp ifExp{{}, {}, pop<asdl::ExprImpl>()};
            ifExp.test = pop<asdl::ExprImpl>();
            ifExp.body = pop<asdl::ExprImpl>();
            outer.push(asdl::ExprImpl{std::move(ifExp)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ConditionalExpression<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ConditionalExpression<Implicit, AsyncFlow, ScopeFlow>,
                        ConditionalExpressionAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct LambDef
          : tao::pegtl::if_must<
                Lambda<Implicit>,
                tao::pegtl::opt<VarargsList<Implicit, AsyncFlow, ScopeFlow>>,
                Colon<Implicit>, Test<Implicit, false, true>> {};
      struct LambDefAction : Stack<asdl::ExprImpl, asdl::Arguments> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Lambda lambda{{}, pop<asdl::ExprImpl>()};
          if (has_value()) {
            lambda.args = pop<asdl::Arguments>();
          }
          outer.push(asdl::ExprImpl{std::move(lambda)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<LambDef<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<LambDef<Implicit, AsyncFlow, ScopeFlow>,
                        LambDefAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Test : tao::pegtl::sor<
                        ConditionalExpression<Implicit, AsyncFlow, ScopeFlow>,
                        LambDef<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct LambDefNoCond;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestNocond
          : tao::pegtl::sor<OrTest<Implicit, AsyncFlow, ScopeFlow>,
                            LambDefNoCond<Implicit, AsyncFlow, ScopeFlow>> {};
      struct TestNocondAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(pop<asdl::ExprImpl>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TestNocond<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<TestNocond<Implicit, AsyncFlow, ScopeFlow>,
                        TestNocondAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct LambDefNoCond
          : tao::pegtl::if_must<
                Lambda<Implicit>,
                tao::pegtl::opt<VarargsList<Implicit, AsyncFlow, ScopeFlow>>,
                Colon<Implicit>, TestNocond<Implicit, false, true>> {};
      struct LambDefNoCondAction : Stack<asdl::ExprImpl, asdl::Arguments> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Lambda lambda{{}, pop<asdl::ExprImpl>()};
          if (has_value()) {
            lambda.args = pop<asdl::Arguments>();
          }
          outer.push(asdl::ExprImpl{std::move(lambda)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<LambDefNoCond<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<LambDefNoCond<Implicit, AsyncFlow, ScopeFlow>,
                        LambDefNoCondAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AndTest;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct OrTest
          : tao::pegtl::list_must<AndTest<Implicit, AsyncFlow, ScopeFlow>,
                                  Or<Implicit>> {};
      struct OrTestAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 1) {
            asdl::Bool asdlBool{asdl::Bool::OR, {}};
            asdlBool.values.resize(s);
            transform(asdlBool.values.begin());
            outer.push(asdl::ExprImpl{std::move(asdlBool)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<OrTest<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<OrTest<Implicit, AsyncFlow, ScopeFlow>, OrTestAction> {
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct NotTest;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AndTest
          : tao::pegtl::list_must<NotTest<Implicit, AsyncFlow, ScopeFlow>,
                                  And<Implicit>> {};
      struct AndTestAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 1) {
            asdl::Bool asdlBool{asdl::Bool::AND, {}};
            asdlBool.values.resize(s);
            transform(asdlBool.values.begin());
            outer.push(asdl::ExprImpl{std::move(asdlBool)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<AndTest<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<AndTest<Implicit, AsyncFlow, ScopeFlow>,
                        AndTestAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Comparison;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct NotTestNot
          : tao::pegtl::if_must<Not<Implicit>,
                                NotTest<Implicit, AsyncFlow, ScopeFlow>> {};
      struct NotTestNotAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{
              asdl::Unary{asdl::Unary::NOT, pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<NotTestNot<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<NotTestNot<Implicit, AsyncFlow, ScopeFlow>,
                        NotTestNotAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct NotTest
          : tao::pegtl::sor<NotTestNot<Implicit, AsyncFlow, ScopeFlow>,
                            Comparison<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit>
      struct CompOp
          : tao::pegtl::sor<Lt<Implicit>, Gt<Implicit>, EqEq<Implicit>,
                            GtE<Implicit>, LtE<Implicit>, NotEq<Implicit>,
                            InOp<Implicit>, NotIn<Implicit>, Is<Implicit>,
                            IsNot<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ComparisonTail
          : tao::pegtl::if_must<CompOp<Implicit>,
                                Expr<Implicit, AsyncFlow, ScopeFlow>> {};
      struct ComparisonTailAction
          : Stack<asdl::ExprImpl, asdl::CompareExpr::Op> {
        template <typename Outer>
        void success(Outer &&outer) {
          [this](auto &&expr) {
            asdl::CompareExpr compareExpr{{},
                                          this->template pop<asdl::ExprImpl>()};
            compareExpr.op = this->template pop<asdl::CompareExpr::Op>();
            if (!std::holds_alternative<asdl::Compare>(*expr.value)) {
              asdl::Compare compare;
              using std::swap;
              swap(expr, compare.left);
              expr = std::move(compare);
            }
            std::get<asdl::Compare>(*expr.value)
                .comparators.push_back(std::move(compareExpr));
          }(outer.template top<asdl::ExprImpl>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ComparisonTail<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ComparisonTail<Implicit, AsyncFlow, ScopeFlow>,
                        ComparisonTailAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Comparison
          : tao::pegtl::seq<Expr<Implicit, AsyncFlow, ScopeFlow>,
                            tao::pegtl::star<ComparisonTail<Implicit, AsyncFlow,
                                                            ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct StarExpr : tao::pegtl::seq<Star<Implicit>,
                                        Expr<Implicit, AsyncFlow, ScopeFlow>> {
      };
      struct StarExprAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{asdl::Starred{pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<StarExpr<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<StarExpr<Implicit, AsyncFlow, ScopeFlow>,
                        StarExprAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct XorExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Expr
          : tao::pegtl::list_must<XorExpr<Implicit, AsyncFlow, ScopeFlow>,
                                  BitOr<Implicit>> {};
      struct ExprAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 1) {
            asdl::Bin bin{asdl::Operator::BIT_OR, {}};
            bin.values.resize(s);
            transform(bin.values.begin());
            outer.push(asdl::ExprImpl{std::move(bin)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<Expr<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<Expr<Implicit, AsyncFlow, ScopeFlow>, ExprAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AndExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct XorExpr
          : tao::pegtl::list_must<AndExpr<Implicit, AsyncFlow, ScopeFlow>,
                                  BitXor<Implicit>> {};
      struct XorExprAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 1) {
            asdl::Bin bin{asdl::Operator::BIT_XOR, {}};
            bin.values.resize(s);
            transform(bin.values.begin());
            outer.push(asdl::ExprImpl{std::move(bin)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<XorExpr<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<XorExpr<Implicit, AsyncFlow, ScopeFlow>,
                        XorExprAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ShiftExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AndExpr
          : tao::pegtl::list_must<ShiftExpr<Implicit, AsyncFlow, ScopeFlow>,
                                  BitAnd<Implicit>> {};
      struct AndExprAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 1) {
            asdl::Bin bin{asdl::Operator::BIT_AND, {}};
            bin.values.resize(s);
            transform(bin.values.begin());
            outer.push(asdl::ExprImpl{std::move(bin)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<AndExpr<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<AndExpr<Implicit, AsyncFlow, ScopeFlow>,
                        AndExprAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArithExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ShiftExprTail
          : tao::pegtl::if_must<
                tao::pegtl::sor<LShift<Implicit>, RShift<Implicit>>,
                ArithExpr<Implicit, AsyncFlow, ScopeFlow>> {};
      struct ShiftExprTailAction : Stack<asdl::ExprImpl, asdl::Operator> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Bin bin{};
          bin.values.resize(2);
          bin.values[1] = pop<asdl::ExprImpl>();
          bin.op = pop<asdl::Operator>();
          bin.values[0] = outer.template pop<asdl::ExprImpl>();
          outer.push(asdl::ExprImpl{std::move(bin)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ShiftExprTail<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ShiftExprTail<Implicit, AsyncFlow, ScopeFlow>,
                        ShiftExprTailAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ShiftExpr
          : tao::pegtl::seq<ArithExpr<Implicit, AsyncFlow, ScopeFlow>,
                            tao::pegtl::star<ShiftExprTail<Implicit, AsyncFlow,
                                                           ScopeFlow>>> {};
      struct ShiftExprAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(pop<asdl::ExprImpl>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ShiftExpr<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ShiftExpr<Implicit, AsyncFlow, ScopeFlow>,
                        ShiftExprAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Term;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArithExprTail
          : tao::pegtl::if_must<tao::pegtl::sor<Add<Implicit>, Sub<Implicit>>,
                                Term<Implicit, AsyncFlow, ScopeFlow>> {};
      struct ArithExprTailAction : Stack<asdl::ExprImpl, asdl::Operator> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Bin bin{};
          bin.values.resize(2);
          bin.values[1] = pop<asdl::ExprImpl>();
          bin.op = pop<asdl::Operator>();
          bin.values[0] = outer.template pop<asdl::ExprImpl>();
          outer.push(asdl::ExprImpl{std::move(bin)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ArithExprTail<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ArithExprTail<Implicit, AsyncFlow, ScopeFlow>,
                        ArithExprTailAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArithExpr
          : tao::pegtl::seq<Term<Implicit, AsyncFlow, ScopeFlow>,
                            tao::pegtl::star<ArithExprTail<Implicit, AsyncFlow,
                                                           ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Factor;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TermTail : tao::pegtl::if_must<
                            tao::pegtl::sor<Mult<Implicit>, MatMult<Implicit>,
                                            Div<Implicit>, Mod<Implicit>,
                                            FloorDiv<Implicit>>,
                            Factor<Implicit, AsyncFlow, ScopeFlow>> {};
      struct TermTailAction : Stack<asdl::ExprImpl, asdl::Operator> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Bin bin{};
          bin.values.resize(2);
          bin.values[1] = pop<asdl::ExprImpl>();
          bin.op = pop<asdl::Operator>();
          bin.values[0] = outer.template pop<asdl::ExprImpl>();
          outer.push(asdl::ExprImpl{std::move(bin)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TermTail<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<TermTail<Implicit, AsyncFlow, ScopeFlow>,
                        TermTailAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Term
          : tao::pegtl::seq<
                Factor<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::star<TermTail<Implicit, AsyncFlow, ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Power;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Factor
          : tao::pegtl::sor<tao::pegtl::if_must<
                                tao::pegtl::sor<UAdd<Implicit>, USub<Implicit>,
                                                BitNot<Implicit>>,
                                Factor<Implicit, AsyncFlow, ScopeFlow>>,
                            Power<Implicit, AsyncFlow, ScopeFlow>> {};
      struct FactorAction : Stack<asdl::ExprImpl, asdl::Unary::Op> {
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
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<Factor<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<Factor<Implicit, AsyncFlow, ScopeFlow>, FactorAction> {
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomExpr;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Power
          : tao::pegtl::seq<
                AtomExpr<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::opt<
                    Pow<Implicit>,
                    tao::pegtl::must<Factor<Implicit, AsyncFlow, ScopeFlow>>>> {
      };
      struct PowerAction : Stack<asdl::ExprImpl, asdl::Operator> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (size() > 1) {
            asdl::Bin bin{};
            bin.values.resize(2);
            bin.values[1] = pop<asdl::ExprImpl>();
            bin.op = pop<asdl::Operator>();
            bin.values[0] = pop<asdl::ExprImpl>();
            outer.push(asdl::ExprImpl{std::move(bin)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<Power<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<Power<Implicit, AsyncFlow, ScopeFlow>, PowerAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Atom;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Trailer;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomExprAtomTrailer
          : tao::pegtl::seq<
                Atom<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::star<Trailer<Implicit, AsyncFlow, ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomExprAwait
          : std::conditional_t<
                AsyncFlow,
                tao::pegtl::if_must<
                    Await<Implicit>,
                    AtomExprAtomTrailer<Implicit, AsyncFlow, ScopeFlow>>,
                tao::pegtl::failure> {};
      struct AtomExprAwaitAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{asdl::Await{pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<AtomExprAwait<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<AtomExprAwait<Implicit, AsyncFlow, ScopeFlow>,
                        AtomExprAwaitAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomExpr
          : tao::pegtl::sor<
                AtomExprAwait<Implicit, AsyncFlow, ScopeFlow>,
                AtomExprAtomTrailer<Implicit, AsyncFlow, ScopeFlow>> {};
      struct AtomExprAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(pop<asdl::ExprImpl>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<AtomExpr<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<AtomExpr<Implicit, AsyncFlow, ScopeFlow>,
                        AtomExprAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestListComp;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLParRParTest
          : tao::pegtl::seq<Test<Implicit, AsyncFlow, ScopeFlow>,
                            tao::pegtl::at<RPar<false>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLParRParTestListComp
          : tao::pegtl::seq<TestListComp<Implicit, AsyncFlow, ScopeFlow>> {};
      struct AtomLParRParTestListCompAction
          : Stack<asdl::ExprImpl, asdl::Comprehension> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (top_is<asdl::Comprehension>()) {
            asdl::GeneratorExp generatorExp;
            generatorExp.generators.reserve(size() - 1);
            while (size() > 1) {
              generatorExp.generators.push_back(pop<asdl::Comprehension>());
            }
            generatorExp.elt = pop<asdl::ExprImpl>();
            outer.push(asdl::ExprImpl{std::move(generatorExp)});
          } else {
            asdl::Tuple tuple;
            tuple.elts.resize(size());
            transform(tuple.elts.begin());
            outer.push(asdl::ExprImpl{std::move(tuple)});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<AtomLParRParTestListComp<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<
                AtomLParRParTestListComp<Implicit, AsyncFlow, ScopeFlow>,
                AtomLParRParTestListCompAction> {};
      struct EmptyTuple : tao::pegtl::success {};
      struct EmptyTupleAction {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{asdl::Tuple{}});
        }
      };
      template <>
      struct Control<EmptyTuple> : ChangeState<EmptyTuple, EmptyTupleAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLParRParContent
          : tao::pegtl::sor<
                YieldExpr<Implicit, AsyncFlow, ScopeFlow>,
                AtomLParRParTest<Implicit, AsyncFlow, ScopeFlow>,
                AtomLParRParTestListComp<Implicit, AsyncFlow, ScopeFlow>,
                EmptyTuple> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLParRPar
          : Paren<Implicit, AsyncFlow, ScopeFlow, AtomLParRParContent> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLBrtRBrt
          : BracketOpt<Implicit, AsyncFlow, ScopeFlow, TestListComp> {};
      struct AtomLBrtRBrtAction : Stack<asdl::ExprImpl, asdl::Comprehension> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (top_is<asdl::Comprehension>()) {
            asdl::ListComp listComp;
            listComp.generators.reserve(size() - 1);
            while (size() > 1) {
              listComp.generators.push_back(pop<asdl::Comprehension>());
            }
            listComp.elt = pop<asdl::ExprImpl>();
            outer.push(asdl::ExprImpl{std::move(listComp)});
          } else {
            asdl::List list;
            list.elts.resize(size());
            transform(list.elts.begin());
            outer.push(asdl::ExprImpl{std::move(list)});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<AtomLBrtRBrt<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<AtomLBrtRBrt<Implicit, AsyncFlow, ScopeFlow>,
                        AtomLBrtRBrtAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictMaker;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SetMaker;
      struct EmptyDict : tao::pegtl::success {};
      struct EmptyDictAction {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{asdl::Dict{}});
        }
      };
      template <>
      struct Control<EmptyDict> : ChangeState<EmptyDict, EmptyDictAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictOrSetMaker
          : tao::pegtl::sor<DictMaker<Implicit, AsyncFlow, ScopeFlow>,
                            SetMaker<Implicit, AsyncFlow, ScopeFlow>,
                            EmptyDict> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AtomLBrcRBrc
          : Brace<Implicit, AsyncFlow, ScopeFlow, DictOrSetMaker> {};
      struct AtomNameImpl : tao::pegtl::minus<NameImpl, Keywords> {};
      template <bool Implicit>
      struct AtomName : Token<Implicit, AtomNameImpl> {};
      struct AtomNameAction : Stack<asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{pop<asdl::Name>()});
        }
      };
      template <bool Implicit>
      struct Control<AtomName<Implicit>>
          : ChangeState<AtomName<Implicit>, AtomNameAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Atom
          : tao::pegtl::sor<AtomLParRPar<Implicit, AsyncFlow, ScopeFlow>,
                            AtomLBrtRBrt<Implicit, AsyncFlow, ScopeFlow>,
                            AtomLBrcRBrc<Implicit, AsyncFlow, ScopeFlow>,
                            STRING<Implicit, AsyncFlow, ScopeFlow>,
                            NUMBER<Implicit>, Ellipsis<Implicit>,
                            None<Implicit>, True<Implicit>, False<Implicit>,
                            AtomName<Implicit>> {};
      struct AtomAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(pop<asdl::ExprImpl>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<Atom<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<Atom<Implicit, AsyncFlow, ScopeFlow>, AtomAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestListComp
          : CompOrCommaList<Implicit, AsyncFlow, ScopeFlow, TestOrStarExpr> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgList;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TrailerLParRPar
          : ParenOpt<Implicit, AsyncFlow, ScopeFlow, ArgList> {};
      struct TrailerLParRParAction : Stack<asdl::ExprImpl, asdl::Keyword> {
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
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TrailerLParRPar<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<TrailerLParRPar<Implicit, AsyncFlow, ScopeFlow>,
                        TrailerLParRParAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptList;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TrailerLBrtRBrt
          : Bracket<Implicit, AsyncFlow, ScopeFlow, SubscriptList> {};
      struct TrailerLBrtRBrtAction : Stack<asdl::SliceImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{asdl::Subscript{
              outer.template pop<asdl::ExprImpl>(), pop<asdl::SliceImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TrailerLBrtRBrt<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<TrailerLBrtRBrt<Implicit, AsyncFlow, ScopeFlow>,
                        TrailerLBrtRBrtAction> {};
      template <bool Implicit>
      struct TrailerPeriod
          : tao::pegtl::if_must<Period<Implicit>, Name<Implicit>> {};
      struct TrailerPeriodAction : Stack<asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{asdl::Attribute{
              outer.template pop<asdl::ExprImpl>(), pop<asdl::Name>()}});
        }
      };
      template <bool Implicit>
      struct Control<TrailerPeriod<Implicit>>
          : ChangeState<TrailerPeriod<Implicit>, TrailerPeriodAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Trailer
          : tao::pegtl::sor<TrailerLParRPar<Implicit, AsyncFlow, ScopeFlow>,
                            TrailerLBrtRBrt<Implicit, AsyncFlow, ScopeFlow>,
                            TrailerPeriod<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Subscript;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptList
          : tao::pegtl::list_tail<Subscript<Implicit, AsyncFlow, ScopeFlow>,
                                  Comma<Implicit>> {};
      struct SubscriptListAction : Stack<asdl::SliceImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 1) {
            asdl::ExtSlice extSlice;
            extSlice.dims.resize(s);
            transform(extSlice.dims.begin());
            outer.push(asdl::SliceImpl{std::move(extSlice)});
          } else {
            outer.push(pop<asdl::SliceImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SubscriptList<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SubscriptList<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptListAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptTest
          : tao::pegtl::seq<Test<Implicit, AsyncFlow, ScopeFlow>,
                            tao::pegtl::not_at<Colon<false>>> {};
      struct SubscriptTestAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::SliceImpl{asdl::Index{pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SubscriptTest<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SubscriptTest<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptTestAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStep
          : tao::pegtl::seq<Colon<Implicit>, Colon<Implicit>,
                            Test<Implicit, AsyncFlow, ScopeFlow>> {};
      struct SubscriptStepAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(
              asdl::SliceImpl{asdl::Slice{{}, {}, pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SubscriptStep<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SubscriptStep<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptStepAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStopStep
          : tao::pegtl::seq<
                Colon<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>,
                Colon<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>> {};
      struct SubscriptStopStepAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Slice slice{{}, {}, pop<asdl::ExprImpl>()};
          slice.upper = pop<asdl::ExprImpl>();
          outer.push(asdl::SliceImpl{std::move(slice)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SubscriptStopStep<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SubscriptStopStep<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptStopStepAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStartStep
          : tao::pegtl::seq<Test<Implicit, AsyncFlow, ScopeFlow>,
                            Colon<Implicit>, Colon<Implicit>,
                            Test<Implicit, AsyncFlow, ScopeFlow>> {};
      struct SubscriptStartStepAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Slice slice{{}, {}, pop<asdl::ExprImpl>()};
          slice.lower = pop<asdl::ExprImpl>();
          outer.push(asdl::SliceImpl{std::move(slice)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SubscriptStartStep<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SubscriptStartStep<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptStartStepAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStartStopStep
          : tao::pegtl::seq<
                Test<Implicit, AsyncFlow, ScopeFlow>, Colon<Implicit>,
                Test<Implicit, AsyncFlow, ScopeFlow>, Colon<Implicit>,
                Test<Implicit, AsyncFlow, ScopeFlow>> {};
      struct SubscriptStartStopStepAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Slice slice{{}, {}, pop<asdl::ExprImpl>()};
          slice.upper = pop<asdl::ExprImpl>();
          slice.lower = pop<asdl::ExprImpl>();
          outer.push(asdl::SliceImpl{std::move(slice)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SubscriptStartStopStep<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SubscriptStartStopStep<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptStartStopStepAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStartStop
          : tao::pegtl::seq<Test<Implicit, AsyncFlow, ScopeFlow>,
                            Colon<Implicit>,
                            Test<Implicit, AsyncFlow, ScopeFlow>,
                            tao::pegtl::opt<Colon<Implicit>>> {};
      struct SubscriptStartStopAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Slice slice{{}, pop<asdl::ExprImpl>(), {}};
          slice.lower = pop<asdl::ExprImpl>();
          outer.push(asdl::SliceImpl{std::move(slice)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SubscriptStartStop<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SubscriptStartStop<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptStartStopAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStop
          : tao::pegtl::seq<Colon<Implicit>,
                            Test<Implicit, AsyncFlow, ScopeFlow>,
                            tao::pegtl::opt<Colon<Implicit>>> {};
      struct SubscriptStopAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(
              asdl::SliceImpl{asdl::Slice{{}, pop<asdl::ExprImpl>(), {}}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SubscriptStop<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SubscriptStop<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptStopAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SubscriptStart
          : tao::pegtl::seq<
                tao::pegtl::opt<Test<Implicit, AsyncFlow, ScopeFlow>>,
                Colon<Implicit>, tao::pegtl::opt<Colon<Implicit>>> {};
      struct SubscriptStartAction : Stack<asdl::ExprImpl> {
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
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SubscriptStart<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SubscriptStart<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptStartAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Subscript
          : tao::pegtl::sor<
                SubscriptTest<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStep<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStopStep<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStartStep<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStartStopStep<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStop<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStartStop<Implicit, AsyncFlow, ScopeFlow>,
                SubscriptStart<Implicit, AsyncFlow, ScopeFlow>> {};
      struct SubscriptAction : Stack<asdl::SliceImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(pop<asdl::SliceImpl>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<Subscript<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<Subscript<Implicit, AsyncFlow, ScopeFlow>,
                        SubscriptAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprList
          : tao::pegtl::list_tail<
                tao::pegtl::sor<Expr<Implicit, AsyncFlow, ScopeFlow>,
                                StarExpr<Implicit, AsyncFlow, ScopeFlow>>,
                Comma<Implicit>> {};
      struct ExprListAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 0) {
            asdl::Tuple tuple;
            tuple.elts.resize(s);
            transform(tuple.elts.begin());
            outer.push(asdl::ExprImpl{std::move(tuple)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ExprList<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ExprList<Implicit, AsyncFlow, ScopeFlow>,
                        ExprListAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TestList
          : tao::pegtl::list_tail<Test<Implicit, AsyncFlow, ScopeFlow>,
                                  Comma<Implicit>> {};
      struct TestListAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 0) {
            asdl::Tuple tuple;
            tuple.elts.resize(s);
            transform(tuple.elts.begin());
            outer.push(asdl::ExprImpl{std::move(tuple)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TestList<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<TestList<Implicit, AsyncFlow, ScopeFlow>,
                        TestListAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct SetMaker
          : CompOrCommaList<Implicit, AsyncFlow, ScopeFlow, TestOrStarExpr> {};
      struct SetMakerAction : Stack<asdl::ExprImpl, asdl::Comprehension> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (top_is<asdl::Comprehension>()) {
            asdl::SetComp setComp;
            setComp.generators.reserve(size());
            while (top_is<asdl::Comprehension>()) {
              setComp.generators.push_back(pop<asdl::Comprehension>());
            }
            setComp.elt = pop<asdl::ExprImpl>();
            outer.push(asdl::ExprImpl{std::move(setComp)});
          } else {
            asdl::Set set;
            set.elts.resize(size());
            transform(set.elts.begin());
            outer.push(asdl::ExprImpl{std::move(set)});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<SetMaker<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<SetMaker<Implicit, AsyncFlow, ScopeFlow>,
                        SetMakerAction> {};
      template <bool Implicit>
      struct DictMakerUnpack : Unpack<Implicit> {};
      struct DictMakerUnpackAction {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{});
        }
      };
      template <bool Implicit>
      struct Control<DictMakerUnpack<Implicit>>
          : ChangeState<DictMakerUnpack<Implicit>, DictMakerUnpackAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictMakerEltStart
          : tao::pegtl::seq<Test<Implicit, AsyncFlow, ScopeFlow>,
                            Colon<Implicit>> {};
      struct DictMakerEltStartAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(pop<asdl::ExprImpl>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<DictMakerEltStart<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<DictMakerEltStart<Implicit, AsyncFlow, ScopeFlow>,
                        DictMakerEltStartAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictMakerElt
          : tao::pegtl::sor<
                tao::pegtl::if_must<DictMakerUnpack<Implicit>,
                                    Expr<Implicit, AsyncFlow, ScopeFlow>>,
                tao::pegtl::if_must<
                    DictMakerEltStart<Implicit, AsyncFlow, ScopeFlow>,
                    Test<Implicit, AsyncFlow, ScopeFlow>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DictMaker
          : CompOrCommaList<Implicit, AsyncFlow, ScopeFlow, DictMakerElt> {};
      struct DictMakerAction : Stack<asdl::ExprImpl, asdl::Comprehension> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (top_is<asdl::Comprehension>()) {
            asdl::DictComp dictComp;
            dictComp.generators.reserve(size());
            while (top_is<asdl::Comprehension>()) {
              dictComp.generators.push_back(pop<asdl::Comprehension>());
            }
            dictComp.value = pop<asdl::ExprImpl>();
            dictComp.key = pop<asdl::ExprImpl>();
            outer.push(asdl::ExprImpl{std::move(dictComp)});
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
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<DictMaker<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<DictMaker<Implicit, AsyncFlow, ScopeFlow>,
                        DictMakerAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Argument;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgList
          : tao::pegtl::list_tail<Argument<Implicit, AsyncFlow, ScopeFlow>,
                                  Comma<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgumentArg
          : tao::pegtl::seq<
                Test<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::opt<CompFor<Implicit, AsyncFlow, ScopeFlow>>> {};
      struct ArgumentArgAction : Stack<asdl::ExprImpl, asdl::Comprehension> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 1) {
            asdl::GeneratorExp generatorExp;
            generatorExp.generators.reserve(s - 1);
            while (size() > 1) {
              generatorExp.generators.push_back(pop<asdl::Comprehension>());
            }
            generatorExp.elt = pop<asdl::ExprImpl>();
            outer.push(asdl::ExprImpl{std::move(generatorExp)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ArgumentArg<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ArgumentArg<Implicit, AsyncFlow, ScopeFlow>,
                        ArgumentArgAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgumentKeyword
          : tao::pegtl::seq<Name<Implicit>, Eq<Implicit>,
                            Test<Implicit, AsyncFlow, ScopeFlow>> {};
      struct ArgumentKeywordAction : Stack<asdl::ExprImpl, asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Keyword keyword{{}, pop<asdl::ExprImpl>()};
          keyword.arg = pop<asdl::Name>();
          outer.push(std::move(keyword));
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ArgumentKeyword<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ArgumentKeyword<Implicit, AsyncFlow, ScopeFlow>,
                        ArgumentKeywordAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgumentKeywordUnpack
          : tao::pegtl::if_must<Unpack<Implicit>,
                                Test<Implicit, AsyncFlow, ScopeFlow>> {};
      struct ArgumentKeywordUnpackAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::Keyword{{}, pop<asdl::ExprImpl>()});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ArgumentKeywordUnpack<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ArgumentKeywordUnpack<Implicit, AsyncFlow, ScopeFlow>,
                        ArgumentKeywordUnpackAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ArgumentArgUnpack
          : tao::pegtl::if_must<Star<Implicit>,
                                Test<Implicit, AsyncFlow, ScopeFlow>> {};
      struct ArgumentArgUnpackAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{asdl::Starred{pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ArgumentArgUnpack<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ArgumentArgUnpack<Implicit, AsyncFlow, ScopeFlow>,
                        ArgumentArgUnpackAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Argument
          : tao::pegtl::sor<
                ArgumentKeywordUnpack<Implicit, AsyncFlow, ScopeFlow>,
                ArgumentArgUnpack<Implicit, AsyncFlow, ScopeFlow>,
                ArgumentKeyword<Implicit, AsyncFlow, ScopeFlow>,
                ArgumentArg<Implicit, AsyncFlow, ScopeFlow>> {};
      struct ArgumentAction : Stack<asdl::ExprImpl, asdl::Keyword> {
        template <typename Outer>
        void success(Outer &&outer) {
          std::visit(outer, pop());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<Argument<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<Argument<Implicit, AsyncFlow, ScopeFlow>,
                        ArgumentAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct CompIf
          : tao::pegtl::if_must<If<Implicit>,
                                TestNocond<Implicit, AsyncFlow, ScopeFlow>> {};
      struct CompIfAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.template top<asdl::Comprehension>().ifs.push_back(
              pop<asdl::ExprImpl>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<CompIf<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<CompIf<Implicit, AsyncFlow, ScopeFlow>, CompIfAction> {
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct CompForAsync
          : std::conditional_t<
                AsyncFlow,
                tao::pegtl::if_must<Async<Implicit>, For<Implicit>,
                                    ExprList<Implicit, AsyncFlow, ScopeFlow>,
                                    In<Implicit>,
                                    OrTest<Implicit, AsyncFlow, ScopeFlow>>,
                tao::pegtl::failure> {};
      struct CompForAsyncAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Comprehension comprehension{
              {}, pop<asdl::ExprImpl>(), {}, true};
          comprehension.target = pop<asdl::ExprImpl>();
          outer.push(std::move(comprehension));
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<CompForAsync<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<CompForAsync<Implicit, AsyncFlow, ScopeFlow>,
                        CompForAsyncAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct CompForSync
          : tao::pegtl::if_must<
                For<Implicit>, ExprList<Implicit, AsyncFlow, ScopeFlow>,
                In<Implicit>, OrTest<Implicit, AsyncFlow, ScopeFlow>> {};
      struct CompForSyncAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Comprehension comprehension{
              {}, pop<asdl::ExprImpl>(), {}, false};
          comprehension.target = pop<asdl::ExprImpl>();
          outer.push(std::move(comprehension));
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<CompForSync<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<CompForSync<Implicit, AsyncFlow, ScopeFlow>,
                        CompForSyncAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct CompFor
          : tao::pegtl::seq<
                tao::pegtl::sor<CompForAsync<Implicit, AsyncFlow, ScopeFlow>,
                                CompForSync<Implicit, AsyncFlow, ScopeFlow>>,
                tao::pegtl::star<tao::pegtl::sor<
                    CompForAsync<Implicit, AsyncFlow, ScopeFlow>,
                    CompForSync<Implicit, AsyncFlow, ScopeFlow>,
                    CompIf<Implicit, AsyncFlow, ScopeFlow>>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldArg;
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldExpr
          : std::conditional_t<
                ScopeFlow || AsyncFlow,
                tao::pegtl::seq<
                    Yield<Implicit>,
                    tao::pegtl::opt<YieldArg<Implicit, AsyncFlow, ScopeFlow>>>,
                tao::pegtl::failure> {};
      struct YieldExprAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (has_value()) {
            outer.push(pop<asdl::ExprImpl>());
          } else {
            outer.push(asdl::ExprImpl{asdl::Yield{}});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<YieldExpr<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<YieldExpr<Implicit, AsyncFlow, ScopeFlow>,
                        YieldExprAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldArgFrom
          : tao::pegtl::if_must<From<Implicit>,
                                Test<Implicit, AsyncFlow, ScopeFlow>> {};
      struct YieldArgFromAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{asdl::YieldFrom{pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<YieldArgFrom<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<YieldArgFrom<Implicit, AsyncFlow, ScopeFlow>,
                        YieldArgFromAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldArgTestList
          : tao::pegtl::seq<TestList<Implicit, AsyncFlow, ScopeFlow>> {};
      struct YieldArgTestListAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::ExprImpl{asdl::Yield{pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<YieldArgTestList<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<YieldArgTestList<Implicit, AsyncFlow, ScopeFlow>,
                        YieldArgTestListAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldArg
          : tao::pegtl::sor<YieldArgFrom<Implicit, AsyncFlow, ScopeFlow>,
                            YieldArgTestList<Implicit, AsyncFlow, ScopeFlow>> {
      };
    } // namespace grammar
  }   // namespace library
} // namespace chimera
