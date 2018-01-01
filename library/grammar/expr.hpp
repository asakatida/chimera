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

#include "asdl/asdl.hpp"
#include "grammar/identifier.hpp"
#include "grammar/keys.hpp"
#include "grammar/number.hpp"
#include "grammar/oper.hpp"
#include "grammar/rules.hpp"
#include "grammar/string.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <typename Option, template <typename> typename Start,
                template <typename> typename Content,
                template <typename> typename End>
      struct GroupOpt
          : IfMust<
                Start<typename Option::template Set<Option::Implicit>>,
                Opt<Content<typename Option::template Set<Option::Implicit>>>,
                End<Option>> {};
      template <typename Option, template <typename> typename Content>
      using Paren =
          IfMust<LPar<typename Option::template Set<Option::Implicit>>,
                 Content<typename Option::template Set<Option::Implicit>>,
                 RPar<Option>>;
      template <typename Option, template <typename> typename Content>
      using ParenOpt = GroupOpt<Option, LPar, Content, RPar>;
      template <typename Option, template <typename> typename Content>
      using Bracket =
          IfMust<LBrt<typename Option::template Set<Option::Implicit>>,
                 Content<typename Option::template Set<Option::Implicit>>,
                 RBrt<Option>>;
      template <typename Option, template <typename> typename Content>
      using BracketOpt = GroupOpt<Option, LBrt, Content, RBrt>;
      template <typename Option, template <typename> typename Content>
      using Brace =
          IfMust<LBrc<typename Option::template Set<Option::Implicit>>,
                 Content<typename Option::template Set<Option::Implicit>>,
                 RBrc<Option>>;
      template <typename Option>
      struct CompFor;
      template <typename Option, template <typename> typename Elt>
      struct CompOrCommaList : Sor<Seq<Elt<Option>, CompFor<Option>>,
                                   ListTail<Elt<Option>, Comma<Option>>> {};
      template <typename Option>
      struct Test;
      template <typename Option>
      struct VarargsListNameEqTest
          : Seq<Name<Option>, Opt<Eq<Option>, Test<Option>>> {
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
      template <typename Option>
      struct VarargsListStarName : Seq<StarOp<Option>, Opt<Name<Option>>> {
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
      template <typename Option>
      struct VarargsListUnpackName
          : Seq<Unpack<Option>, Name<Option>, Opt<Comma<Option>>> {
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
      template <typename Option>
      struct VarargsList
          : Sor<IfMust<
                    VarargsListNameEqTest<Option>,
                    Star<Comma<Option>, VarargsListNameEqTest<Option>>,
                    Opt<Comma<Option>,
                        Opt<Sor<IfMust<VarargsListStarName<Option>,
                                       Star<Comma<Option>,
                                            VarargsListNameEqTest<Option>>,
                                       Opt<Comma<Option>,
                                           Opt<VarargsListUnpackName<Option>>>>,
                                VarargsListUnpackName<Option>>>>>,
                IfMust<VarargsListStarName<Option>,
                       Star<Comma<Option>, VarargsListNameEqTest<Option>>,
                       Opt<Comma<Option>, Opt<VarargsListUnpackName<Option>>>>,
                VarargsListUnpackName<Option>> {
        struct Transform : rules::Stack<asdl::Arguments> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::Arguments>());
          }
        };
      };
      template <typename Option>
      struct StarExpr;
      template <typename Option>
      struct TestOrStarExpr : Sor<Test<Option>, StarExpr<Option>> {};
      template <typename Option>
      struct TestListStarExpr
          : ListTail<TestOrStarExpr<Option>, Comma<Option>> {};
      template <typename Option>
      struct ExprList;
      template <typename Option>
      struct OrTest;
      template <typename Option>
      struct ConditionalExpression
          : Seq<OrTest<Option>,
                Opt<If<Option>,
                    Must<OrTest<Option>, Else<Option>, Test<Option>>>> {
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
      template <typename Option>
      struct LambDef
          : IfMust<Lambda<Option>, Opt<VarargsList<Option>>, Colon<Option>,
                   Test<typename Option::template UnSet<
                       Option::AsyncFlow>::template Set<Option::ScopeFlow>>> {
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
      template <typename Option>
      struct Test : Sor<ConditionalExpression<Option>, LambDef<Option>> {};
      template <typename Option>
      struct LambDefNoCond;
      template <typename Option>
      struct TestNocond : Sor<OrTest<Option>, LambDefNoCond<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
        };
      };
      template <typename Option>
      struct LambDefNoCond
          : IfMust<Lambda<Option>, Opt<VarargsList<Option>>, Colon<Option>,
                   TestNocond<typename Option::template UnSet<
                       Option::AsyncFlow>::template Set<Option::ScopeFlow>>> {
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
      template <typename Option>
      struct AndTest;
      template <typename Option>
      struct OrTest : ListMust<AndTest<Option>, Or<Option>> {
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
      template <typename Option>
      struct NotTest;
      template <typename Option>
      struct AndTest : ListMust<NotTest<Option>, And<Option>> {
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
      template <typename Option>
      struct Comparison;
      template <typename Option>
      struct NotTestNot : IfMust<Not<Option>, NotTest<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{
                asdl::Unary{asdl::Unary::NOT, pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct NotTest : Sor<NotTestNot<Option>, Comparison<Option>> {};
      template <typename Option>
      struct CompOp : Sor<Lt<Option>, Gt<Option>, EqEq<Option>, GtE<Option>,
                          LtE<Option>, NotEq<Option>, InOp<Option>,
                          NotIn<Option>, Is<Option>, IsNot<Option>> {};
      template <typename Option>
      struct ComparisonTail : IfMust<CompOp<Option>, Expr<Option>> {
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
      template <typename Option>
      struct Comparison : Seq<Expr<Option>, Star<ComparisonTail<Option>>> {};
      template <typename Option>
      struct StarExpr : Seq<StarOp<Option>, Expr<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Starred{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct XorExpr;
      template <typename Option>
      struct Expr : ListMust<XorExpr<Option>, BitOr<Option>> {
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
      template <typename Option>
      struct AndExpr;
      template <typename Option>
      struct XorExpr : ListMust<AndExpr<Option>, BitXor<Option>> {
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
      template <typename Option>
      struct ShiftExpr;
      template <typename Option>
      struct AndExpr : ListMust<ShiftExpr<Option>, BitAnd<Option>> {
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
      template <typename Option>
      struct ArithExpr;
      template <typename Option>
      struct ShiftExprTail
          : IfMust<Sor<LShift<Option>, RShift<Option>>, ArithExpr<Option>> {
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
      template <typename Option>
      struct ShiftExpr : Seq<ArithExpr<Option>, Star<ShiftExprTail<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
        };
      };
      template <typename Option>
      struct Term;
      template <typename Option>
      struct ArithExprTail
          : IfMust<Sor<Add<Option>, Sub<Option>>, Term<Option>> {
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
      template <typename Option>
      struct ArithExpr : Seq<Term<Option>, Star<ArithExprTail<Option>>> {};
      template <typename Option>
      struct Factor;
      template <typename Option>
      struct TermTail : IfMust<Sor<Mult<Option>, MatMult<Option>, Div<Option>,
                                   Mod<Option>, FloorDiv<Option>>,
                               Factor<Option>> {
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
      template <typename Option>
      struct Term : Seq<Factor<Option>, Star<TermTail<Option>>> {};
      template <typename Option>
      struct Power;
      template <typename Option>
      struct Factor
          : Sor<IfMust<Sor<UAdd<Option>, USub<Option>, BitNot<Option>>,
                       Factor<Option>>,
                Power<Option>> {
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
      template <typename Option>
      struct AtomExpr;
      template <typename Option>
      struct Power
          : Seq<AtomExpr<Option>, Opt<Pow<Option>, Must<Factor<Option>>>> {
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
      template <typename Option>
      struct Atom;
      template <typename Option>
      struct Trailer;
      template <typename Option>
      struct AtomExprAtomTrailer : Seq<Atom<Option>, Star<Trailer<Option>>> {};
      template <typename Option>
      struct AtomExprAwait
          : std::conditional_t<
                Option::template Get<Option::AsyncFlow>,
                IfMust<Await<Option>, AtomExprAtomTrailer<Option>>, Failure> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Await{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct AtomExpr
          : Sor<AtomExprAwait<Option>, AtomExprAtomTrailer<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
        };
      };
      template <typename Option>
      struct TestListComp;
      template <typename Option>
      struct AtomLParRParTest : Seq<Test<Option>, At<RPar<Option>>> {};
      template <typename Option>
      struct AtomLParRParTestListComp : Seq<TestListComp<Option>> {
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
      template <typename Option>
      struct AtomLParRParContent
          : Sor<YieldExpr<Option>, AtomLParRParTest<Option>,
                AtomLParRParTestListComp<Option>, EmptyTuple> {};
      template <typename Option>
      struct AtomLParRPar : Paren<Option, AtomLParRParContent> {};
      template <typename Option>
      struct AtomLBrtRBrt : BracketOpt<Option, TestListComp> {
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
      template <typename Option>
      struct DictMaker;
      template <typename Option>
      struct SetMaker;
      struct EmptyDict : Success {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Dict{}});
          }
        };
      };
      template <typename Option>
      struct DictOrSetMaker
          : Sor<DictMaker<Option>, SetMaker<Option>, EmptyDict> {};
      template <typename Option>
      struct AtomLBrcRBrc : Brace<Option, DictOrSetMaker> {};
      template <typename Option>
      struct AtomName : Minus<Name<Option>, Keywords<Option>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{pop<asdl::Name>()});
          }
        };
      };
      template <typename Option>
      struct Atom : Sor<AtomLParRPar<Option>, AtomLBrtRBrt<Option>,
                        AtomLBrcRBrc<Option>, STRING<Option>, NUMBER<Option>,
                        EllipsisOp<Option>, None<Option>, True<Option>,
                        False<Option>, AtomName<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::ExprImpl>());
          }
        };
      };
      template <typename Option>
      struct TestListComp : CompOrCommaList<Option, TestOrStarExpr> {};
      template <typename Option>
      struct ArgList;
      template <typename Option>
      struct TrailerLParRPar : ParenOpt<Option, ArgList> {
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
      template <typename Option>
      struct SubscriptList;
      template <typename Option>
      struct TrailerLBrtRBrt : Bracket<Option, SubscriptList> {
        struct Transform : rules::Stack<asdl::SliceImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Subscript{
                outer.template pop<asdl::ExprImpl>(), pop<asdl::SliceImpl>()}});
          }
        };
      };
      template <typename Option>
      struct TrailerPeriod : IfMust<Period<Option>, Name<Option>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Attribute{
                outer.template pop<asdl::ExprImpl>(), pop<asdl::Name>()}});
          }
        };
      };
      template <typename Option>
      struct Trailer : Sor<TrailerLParRPar<Option>, TrailerLBrtRBrt<Option>,
                           TrailerPeriod<Option>> {};
      template <typename Option>
      struct Subscript;
      template <typename Option>
      struct SubscriptList : ListTail<Subscript<Option>, Comma<Option>> {
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
      template <typename Option>
      struct SubscriptTest : Seq<Test<Option>, NotAt<Colon<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::SliceImpl{asdl::Index{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct SubscriptStep : Seq<Colon<Option>, Colon<Option>, Test<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(
                asdl::SliceImpl{asdl::Slice{{}, {}, pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct SubscriptStopStep
          : Seq<Colon<Option>, Test<Option>, Colon<Option>, Test<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Slice slice{{}, {}, pop<asdl::ExprImpl>()};
            slice.upper = pop<asdl::ExprImpl>();
            outer.push(asdl::SliceImpl{std::move(slice)});
          }
        };
      };
      template <typename Option>
      struct SubscriptStartStep
          : Seq<Test<Option>, Colon<Option>, Colon<Option>, Test<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Slice slice{{}, {}, pop<asdl::ExprImpl>()};
            slice.lower = pop<asdl::ExprImpl>();
            outer.push(asdl::SliceImpl{std::move(slice)});
          }
        };
      };
      template <typename Option>
      struct SubscriptStartStopStep
          : Seq<Test<Option>, Colon<Option>, Test<Option>, Colon<Option>,
                Test<Option>> {
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
      template <typename Option>
      struct SubscriptStartStop
          : Seq<Test<Option>, Colon<Option>, Test<Option>, Opt<Colon<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Slice slice{{}, pop<asdl::ExprImpl>(), {}};
            slice.lower = pop<asdl::ExprImpl>();
            outer.push(asdl::SliceImpl{std::move(slice)});
          }
        };
      };
      template <typename Option>
      struct SubscriptStop
          : Seq<Colon<Option>, Test<Option>, Opt<Colon<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(
                asdl::SliceImpl{asdl::Slice{{}, pop<asdl::ExprImpl>(), {}}});
          }
        };
      };
      template <typename Option>
      struct SubscriptStart
          : Seq<Opt<Test<Option>>, Colon<Option>, Opt<Colon<Option>>> {
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
      template <typename Option>
      struct Subscript
          : Sor<SubscriptTest<Option>, SubscriptStep<Option>,
                SubscriptStopStep<Option>, SubscriptStartStep<Option>,
                SubscriptStartStopStep<Option>, SubscriptStop<Option>,
                SubscriptStartStop<Option>, SubscriptStart<Option>> {
        struct Transform : rules::Stack<asdl::SliceImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::SliceImpl>());
          }
        };
      };
      template <typename Option>
      struct ExprList
          : ListTail<Sor<Expr<Option>, StarExpr<Option>>, Comma<Option>> {
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
      template <typename Option>
      struct TestList : ListTail<Test<Option>, Comma<Option>> {
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
      template <typename Option>
      struct SetMaker : CompOrCommaList<Option, TestOrStarExpr> {
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
      template <typename Option>
      struct DictMakerUnpack : Unpack<Option> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::UnpackDict{}});
          }
        };
      };
      template <typename Option>
      struct DictMakerEltStart : Seq<Test<Option>, Colon<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::visit(outer, pop());
          }
        };
      };
      template <typename Option>
      struct DictMakerElt
          : Sor<IfMust<DictMakerUnpack<Option>, Expr<Option>>,
                IfMust<DictMakerEltStart<Option>, Test<Option>>> {};
      template <typename Option>
      struct DictMaker : CompOrCommaList<Option, DictMakerElt> {
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
      template <typename Option>
      struct Argument;
      template <typename Option>
      struct ArgList : ListTail<Argument<Option>, Comma<Option>> {};
      template <typename Option>
      struct ArgumentArg : Seq<Test<Option>, Opt<CompFor<Option>>> {
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
      template <typename Option>
      struct ArgumentKeyword : Seq<Name<Option>, Eq<Option>, Test<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Keyword keyword{{}, pop<asdl::ExprImpl>()};
            keyword.arg = pop<asdl::Name>();
            outer.push(std::move(keyword));
          }
        };
      };
      template <typename Option>
      struct ArgumentKeywordUnpack : IfMust<Unpack<Option>, Test<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Keyword{{}, pop<asdl::ExprImpl>()});
          }
        };
      };
      template <typename Option>
      struct ArgumentArgUnpack : IfMust<StarOp<Option>, Test<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Starred{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct Argument
          : Sor<ArgumentKeywordUnpack<Option>, ArgumentArgUnpack<Option>,
                ArgumentKeyword<Option>, ArgumentArg<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Keyword> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::visit(outer, pop());
          }
        };
      };
      template <typename Option>
      struct CompIf : IfMust<If<Option>, TestNocond<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.template top<asdl::Comprehension>().ifs.push_back(
                pop<asdl::ExprImpl>());
          }
        };
      };
      template <typename Option>
      struct CompForAsync
          : std::conditional_t<
                Option::template Get<Option::AsyncFlow>,
                IfMust<Async<Option>, For<Option>, ExprList<Option>, In<Option>,
                       OrTest<Option>>,
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
      template <typename Option>
      struct CompForSync
          : IfMust<For<Option>, ExprList<Option>, In<Option>, OrTest<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            auto iter = pop<asdl::ExprImpl>();
            auto target = pop<asdl::ExprImpl>();
            outer.push(asdl::Comprehension{target, iter, {}, false});
          }
        };
      };
      template <typename Option>
      struct CompFor : Seq<Sor<CompForAsync<Option>, CompForSync<Option>>,
                           Star<Sor<CompForAsync<Option>, CompForSync<Option>,
                                    CompIf<Option>>>> {};
      template <typename Option>
      struct YieldArgFrom : IfMust<From<Option>, Test<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::YieldFrom{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct YieldArgTestList : Seq<TestList<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::ExprImpl{asdl::Yield{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct YieldArg : Sor<YieldArgFrom<Option>, YieldArgTestList<Option>> {};
      template <typename Option>
      struct YieldExpr
          : std::conditional_t<
                Option::template Get<Option::AsyncFlow, Option::ScopeFlow>,
                Seq<Yield<Option>, Opt<YieldArg<Option>>>, Failure> {
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
    } // namespace grammar
  }   // namespace library
} // namespace chimera
