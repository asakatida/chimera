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
#include "grammar/flags.hpp"
#include "grammar/identifier.hpp"
#include "grammar/keys.hpp"
#include "grammar/number.hpp"
#include "grammar/oper.hpp"
#include "grammar/rules.hpp"
#include "grammar/string.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <flags::Flag Option>
      struct Atom;
      template <flags::Flag Option>
      struct Argument;
      template <flags::Flag Option>
      using ArgList = list_tail<Argument<flags::mask<Option, flags::IMPLICIT>>,
                                Comma<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct TrailerLParRPar
          : if_must<LPar<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                    flags::IMPLICIT>>,
                    opt<ArgList<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                           flags::IMPLICIT>>>,
                    RPar<flags::mask<Option, flags::IMPLICIT>>> {
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
            outer.push(std::move(call));
          }
        };
      };
      template <flags::Flag Option>
      struct Subscript;
      template <flags::Flag Option>
      struct SubscriptListOne : list_tail<Subscript<Option>, Comma<Option>> {
        using Transform = rules::VectorCapture<asdl::SliceImpl>;
      };
      template <flags::Flag Option>
      struct SubscriptList
          : if_must<LBrt<flags::set<Option, flags::IMPLICIT>>,
                    SubscriptListOne<flags::set<Option, flags::IMPLICIT>>,
                    RBrt<Option>> {
        using Transform =
            rules::ReshapeCapture<asdl::ExtSlice, std::vector<asdl::SliceImpl>>;
      };
      template <flags::Flag Option>
      using SubscriptSingle =
          seq<LBrt<flags::set<Option, flags::IMPLICIT>>,
              Subscript<flags::set<Option, flags::IMPLICIT>>, RBrt<Option>>;
      template <flags::Flag Option>
      struct TrailerLBrtRBrt
          : sor<SubscriptSingle<Option>, SubscriptList<Option>> {
        struct Transform : rules::Stack<asdl::SliceImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Subscript{outer.template pop<asdl::ExprImpl>(),
                                       pop<asdl::SliceImpl>()});
          }
        };
      };
      template <flags::Flag Option>
      struct TrailerPeriod
          : if_must<Period<flags::mask<Option, flags::IMPLICIT>>,
                    Name<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Attribute{outer.template pop<asdl::ExprImpl>(),
                                       pop<asdl::Name>()});
          }
        };
      };
      template <flags::Flag Option>
      using Trailer = sor<TrailerLParRPar<flags::mask<Option, flags::IMPLICIT>>,
                          TrailerLBrtRBrt<flags::mask<Option, flags::IMPLICIT>>,
                          TrailerPeriod<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using AtomExprAtomTrailer =
          seq<Atom<flags::mask<Option, flags::IMPLICIT>>,
              star<Trailer<flags::mask<Option, flags::IMPLICIT>>>>;
      template <flags::Flag Option>
      struct AtomExprAwait
          : std::conditional_t<
                flags::get<Option, flags::ASYNC_FLOW>,
                if_must<
                    Await<flags::mask<Option, flags::IMPLICIT>>,
                    AtomExprAtomTrailer<flags::mask<Option, flags::IMPLICIT>>>,
                failure> {
        using Transform = rules::ReshapeCapture<asdl::Await, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct AtomExpr
          : sor<AtomExprAwait<flags::mask<Option, flags::IMPLICIT>>,
                AtomExprAtomTrailer<flags::mask<Option, flags::IMPLICIT>>> {};
      template <flags::Flag Option>
      struct Power;
      template <flags::Flag Option>
      struct Factor;
      template <flags::Flag Option>
      struct Unary : if_must<sor<UAdd<flags::mask<Option, flags::IMPLICIT>>,
                                 USub<flags::mask<Option, flags::IMPLICIT>>,
                                 BitNot<flags::mask<Option, flags::IMPLICIT>>>,
                             Factor<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform =
            rules::ReshapeCapture<asdl::Unary, asdl::Unary::Op, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct Factor : sor<Unary<flags::mask<Option, flags::IMPLICIT>>,
                          Power<flags::mask<Option, flags::IMPLICIT>>> {};
      template <flags::Flag Option>
      struct TermTail
          : if_must<sor<Mult<flags::mask<Option, flags::IMPLICIT>>,
                        MatMult<flags::mask<Option, flags::IMPLICIT>>,
                        Div<flags::mask<Option, flags::IMPLICIT>>,
                        Mod<flags::mask<Option, flags::IMPLICIT>>,
                        FloorDiv<flags::mask<Option, flags::IMPLICIT>>>,
                    Factor<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Bin bin;
            bin.values = {outer.template pop<asdl::ExprImpl>(),
                          pop<asdl::ExprImpl>()};
            bin.op = pop<asdl::Operator>();
            outer.push(std::move(bin));
          }
        };
      };
      template <flags::Flag Option>
      using Term = seq<Factor<flags::mask<Option, flags::IMPLICIT>>,
                       star<TermTail<flags::mask<Option, flags::IMPLICIT>>>>;
      template <flags::Flag Option>
      struct ArithExprTail
          : if_must<sor<Add<flags::mask<Option, flags::IMPLICIT>>,
                        Sub<flags::mask<Option, flags::IMPLICIT>>>,
                    Term<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Bin bin;
            bin.values = {outer.template pop<asdl::ExprImpl>(),
                          pop<asdl::ExprImpl>()};
            bin.op = pop<asdl::Operator>();
            outer.push(std::move(bin));
          }
        };
      };
      template <flags::Flag Option>
      using ArithExpr =
          seq<Term<flags::mask<Option, flags::IMPLICIT>>,
              star<ArithExprTail<flags::mask<Option, flags::IMPLICIT>>>>;
      template <flags::Flag Option>
      struct ShiftExprTail
          : if_must<sor<LShift<flags::mask<Option, flags::IMPLICIT>>,
                        RShift<flags::mask<Option, flags::IMPLICIT>>>,
                    ArithExpr<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Bin bin;
            bin.values = {outer.template pop<asdl::ExprImpl>(),
                          pop<asdl::ExprImpl>()};
            bin.op = pop<asdl::Operator>();
            outer.push(std::move(bin));
          }
        };
      };
      template <flags::Flag Option>
      struct ShiftExpr
          : seq<ArithExpr<flags::mask<Option, flags::IMPLICIT>>,
                star<ShiftExprTail<flags::mask<Option, flags::IMPLICIT>>>> {};
      template <flags::Flag Option>
      struct AndExpr
          : list_must<ShiftExpr<flags::mask<Option, flags::IMPLICIT>>,
                      BitAnd<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bin bin{asdl::Operator::BIT_AND, {}};
              bin.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(bin.values));
              outer.push(std::move(bin));
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct XorExpr : list_must<AndExpr<flags::mask<Option, flags::IMPLICIT>>,
                                 BitXor<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bin bin{asdl::Operator::BIT_XOR, {}};
              bin.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(bin.values));
              outer.push(std::move(bin));
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct Expr : list_must<XorExpr<flags::mask<Option, flags::IMPLICIT>>,
                              BitOr<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bin bin{asdl::Operator::BIT_OR, {}};
              bin.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(bin.values));
              outer.push(std::move(bin));
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      using CompOp = sor<Lt<flags::mask<Option, flags::IMPLICIT>>,
                         Gt<flags::mask<Option, flags::IMPLICIT>>,
                         EqEq<flags::mask<Option, flags::IMPLICIT>>,
                         GtE<flags::mask<Option, flags::IMPLICIT>>,
                         LtE<flags::mask<Option, flags::IMPLICIT>>,
                         NotEq<flags::mask<Option, flags::IMPLICIT>>,
                         InOp<flags::mask<Option, flags::IMPLICIT>>,
                         NotIn<flags::mask<Option, flags::IMPLICIT>>,
                         Is<flags::mask<Option, flags::IMPLICIT>>,
                         IsNot<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct ComparisonTail
          : if_must<CompOp<flags::mask<Option, flags::IMPLICIT>>,
                    Expr<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::CompareExpr::Op> {
          template <typename Outer>
          void success(Outer &&outer) {
            auto expr = outer.template pop<asdl::ExprImpl>();
            if (!std::holds_alternative<asdl::Compare>(*expr.value)) {
              return outer.push(asdl::Compare{
                  std::move(expr),
                  {reshape<asdl::CompareExpr, asdl::CompareExpr::Op,
                           asdl::ExprImpl>()}});
            }
            std::get<asdl::Compare>(*expr.value)
                .comparators.push_back(
                    reshape<asdl::CompareExpr, asdl::CompareExpr::Op,
                            asdl::ExprImpl>());
            outer.push(std::move(expr));
          }
        };
      };
      template <flags::Flag Option>
      using Comparison =
          seq<Expr<flags::mask<Option, flags::IMPLICIT>>,
              star<ComparisonTail<flags::mask<Option, flags::IMPLICIT>>>>;
      template <flags::Flag Option>
      struct DictMakerUnpack : Unpack<flags::mask<Option, flags::IMPLICIT>> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::UnpackDict{});
          }
        };
      };
      template <flags::Flag Option>
      struct NotTestNot
          : if_must<Not<flags::mask<Option, flags::IMPLICIT>>,
                    sor<NotTestNot<flags::mask<Option, flags::IMPLICIT>>,
                        Comparison<flags::mask<Option, flags::IMPLICIT>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Unary{asdl::Unary::NOT, pop<asdl::ExprImpl>()});
          }
        };
      };
      template <flags::Flag Option>
      using NotTest = sor<NotTestNot<flags::mask<Option, flags::IMPLICIT>>,
                          Comparison<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct AndTest : list_must<NotTest<flags::mask<Option, flags::IMPLICIT>>,
                                 And<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bool asdlBool{asdl::Bool::AND, {}};
              asdlBool.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(asdlBool.values));
              outer.push(std::move(asdlBool));
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct OrTest : list_must<AndTest<flags::mask<Option, flags::IMPLICIT>>,
                                Or<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              asdl::Bool asdlBool{asdl::Bool::OR, {}};
              asdlBool.values.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(asdlBool.values));
              outer.push(std::move(asdlBool));
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct ConditionalExpression;
      template <flags::Flag Option>
      struct LambDef;
      template <flags::Flag Option>
      using Test =
          sor<ConditionalExpression<flags::mask<Option, flags::IMPLICIT>>,
              LambDef<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct ConditionalExpression
          : seq<OrTest<flags::mask<Option, flags::IMPLICIT>>,
                opt<If<flags::mask<Option, flags::IMPLICIT>>,
                    must<OrTest<flags::mask<Option, flags::IMPLICIT>>,
                         Else<flags::mask<Option, flags::IMPLICIT>>,
                         Test<flags::mask<Option, flags::IMPLICIT>>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              outer.push(reshape<asdl::IfExp, asdl::ExprImpl, asdl::ExprImpl,
                                 asdl::ExprImpl>());
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct VarargsList;
      template <flags::Flag Option>
      struct LambDef
          : if_must<Lambda<flags::mask<Option, flags::IMPLICIT>>,
                    opt<VarargsList<flags::mask<Option, flags::IMPLICIT>>>,
                    Colon<flags::mask<Option, flags::IMPLICIT>>,
                    Test<flags::set<
                        flags::unSet<flags::mask<Option, flags::IMPLICIT>,
                                     flags::ASYNC_FLOW>,
                        flags::SCOPE_FLOW>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Arguments> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Lambda lambda{{}, pop<asdl::ExprImpl>()};
            if (has_value()) {
              lambda.args = pop<asdl::Arguments>();
            }
            outer.push(std::move(lambda));
          }
        };
      };
      template <flags::Flag Option>
      struct DictMakerEltStart
          : seq<Test<flags::mask<Option, flags::IMPLICIT>>,
                Colon<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::VariantCapture<asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      using DictMakerElt =
          sor<if_must<DictMakerUnpack<flags::mask<Option, flags::IMPLICIT>>,
                      Expr<flags::mask<Option, flags::IMPLICIT>>>,
              if_must<DictMakerEltStart<flags::mask<Option, flags::IMPLICIT>>,
                      Test<flags::mask<Option, flags::IMPLICIT>>>>;
      template <flags::Flag Option>
      struct StarExpr : seq<StarOp<flags::mask<Option, flags::IMPLICIT>>,
                            Expr<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Starred{pop<asdl::ExprImpl>()});
          }
        };
      };
      template <flags::Flag Option>
      struct ExprList
          : list_tail<sor<Expr<flags::mask<Option, flags::IMPLICIT>>,
                          StarExpr<flags::mask<Option, flags::IMPLICIT>>>,
                      Comma<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 0) {
              asdl::Tuple tuple;
              tuple.elts.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
              outer.push(std::move(tuple));
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct Power
          : seq<AtomExpr<flags::mask<Option, flags::IMPLICIT>>,
                opt<Pow<flags::mask<Option, flags::IMPLICIT>>,
                    must<Factor<flags::mask<Option, flags::IMPLICIT>>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              asdl::Bin bin;
              bin.values.reserve(2);
              {
                auto expr = pop<asdl::ExprImpl>();
                bin.op = pop<asdl::Operator>();
                bin.values.push_back(pop<asdl::ExprImpl>());
                bin.values.push_back(std::move(expr));
              }
              outer.push(std::move(bin));
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct CompForAsync
          : std::conditional_t<
                flags::get<Option, flags::ASYNC_FLOW>,
                if_must<Async<flags::mask<Option, flags::IMPLICIT>>,
                        For<flags::mask<Option, flags::IMPLICIT>>,
                        ExprList<flags::mask<Option, flags::IMPLICIT>>,
                        In<flags::mask<Option, flags::IMPLICIT>>,
                        OrTest<flags::mask<Option, flags::IMPLICIT>>>,
                failure> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            auto iter = pop<asdl::ExprImpl>();
            outer.push(asdl::Comprehension{
                pop<asdl::ExprImpl>(), std::move(iter), {}, true});
          }
        };
      };
      template <flags::Flag Option>
      struct CompForSync
          : if_must<For<flags::mask<Option, flags::IMPLICIT>>,
                    ExprList<flags::mask<Option, flags::IMPLICIT>>,
                    In<flags::mask<Option, flags::IMPLICIT>>,
                    OrTest<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            auto iter = pop<asdl::ExprImpl>();
            outer.push(asdl::Comprehension{
                pop<asdl::ExprImpl>(), std::move(iter), {}, false});
          }
        };
      };
      template <flags::Flag Option>
      struct CompIf;
      template <flags::Flag Option>
      using CompFor =
          seq<sor<CompForAsync<flags::mask<Option, flags::IMPLICIT>>,
                  CompForSync<flags::mask<Option, flags::IMPLICIT>>>,
              star<sor<CompForAsync<flags::mask<Option, flags::IMPLICIT>>,
                       CompForSync<flags::mask<Option, flags::IMPLICIT>>,
                       CompIf<flags::mask<Option, flags::IMPLICIT>>>>>;
      template <flags::Flag Option, template <flags::Flag> typename Elt>
      using CompOrCommaList =
          sor<seq<Elt<flags::mask<Option, flags::IMPLICIT>>,
                  CompFor<flags::mask<Option, flags::IMPLICIT>>>,
              list_tail<Elt<flags::mask<Option, flags::IMPLICIT>>,
                        Comma<flags::mask<Option, flags::IMPLICIT>>>>;
      template <flags::Flag Option>
      struct VarargsListNameEqTest
          : seq<Name<flags::mask<Option, flags::IMPLICIT>>,
                opt<Eq<flags::mask<Option, flags::IMPLICIT>>,
                    Test<flags::mask<Option, flags::IMPLICIT>>>> {
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
      template <flags::Flag Option>
      struct VarargsListStarName
          : seq<StarOp<flags::mask<Option, flags::IMPLICIT>>,
                opt<Name<flags::mask<Option, flags::IMPLICIT>>>> {
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
      template <flags::Flag Option>
      struct VarargsListUnpackName
          : seq<Unpack<flags::mask<Option, flags::IMPLICIT>>,
                Name<flags::mask<Option, flags::IMPLICIT>>,
                opt<Comma<flags::mask<Option, flags::IMPLICIT>>>> {
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
      template <flags::Flag Option>
      struct VarargsList
          : sor<if_must<
                    VarargsListNameEqTest<flags::mask<Option, flags::IMPLICIT>>,
                    star<Comma<flags::mask<Option, flags::IMPLICIT>>,
                         VarargsListNameEqTest<
                             flags::mask<Option, flags::IMPLICIT>>>,
                    opt<Comma<flags::mask<Option, flags::IMPLICIT>>,
                        opt<sor<
                            if_must<
                                VarargsListStarName<
                                    flags::mask<Option, flags::IMPLICIT>>,
                                star<
                                    Comma<flags::mask<Option, flags::IMPLICIT>>,
                                    VarargsListNameEqTest<
                                        flags::mask<Option, flags::IMPLICIT>>>,
                                opt<Comma<flags::mask<Option, flags::IMPLICIT>>,
                                    opt<VarargsListUnpackName<flags::mask<
                                        Option, flags::IMPLICIT>>>>>,
                            VarargsListUnpackName<
                                flags::mask<Option, flags::IMPLICIT>>>>>>,
                if_must<
                    VarargsListStarName<flags::mask<Option, flags::IMPLICIT>>,
                    star<Comma<flags::mask<Option, flags::IMPLICIT>>,
                         VarargsListNameEqTest<
                             flags::mask<Option, flags::IMPLICIT>>>,
                    opt<Comma<flags::mask<Option, flags::IMPLICIT>>,
                        opt<VarargsListUnpackName<
                            flags::mask<Option, flags::IMPLICIT>>>>>,
                VarargsListUnpackName<flags::mask<Option, flags::IMPLICIT>>> {};
      template <flags::Flag Option>
      using TestOrStarExpr =
          sor<Test<flags::mask<Option, flags::IMPLICIT>>,
              StarExpr<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using TestListComp =
          CompOrCommaList<flags::mask<Option, flags::IMPLICIT>, TestOrStarExpr>;
      template <flags::Flag Option>
      using TestListStarExpr =
          list_tail<TestOrStarExpr<flags::mask<Option, flags::IMPLICIT>>,
                    Comma<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct ExprList;
      template <flags::Flag Option>
      struct LambDefNoCond;
      template <flags::Flag Option>
      struct TestNocond
          : sor<OrTest<flags::mask<Option, flags::IMPLICIT>>,
                LambDefNoCond<flags::mask<Option, flags::IMPLICIT>>> {};
      template <flags::Flag Option>
      struct LambDefNoCond
          : if_must<Lambda<flags::mask<Option, flags::IMPLICIT>>,
                    opt<VarargsList<flags::mask<Option, flags::IMPLICIT>>>,
                    Colon<flags::mask<Option, flags::IMPLICIT>>,
                    TestNocond<flags::set<
                        flags::unSet<flags::mask<Option, flags::IMPLICIT>,
                                     flags::ASYNC_FLOW>,
                        flags::SCOPE_FLOW>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Arguments> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Lambda lambda{{}, pop<asdl::ExprImpl>()};
            if (has_value()) {
              lambda.args = pop<asdl::Arguments>();
            }
            outer.push(std::move(lambda));
          }
        };
      };
      template <flags::Flag Option>
      using AtomLParRParTest =
          seq<Test<flags::mask<Option, flags::IMPLICIT>>,
              at<RPar<flags::mask<Option, flags::IMPLICIT>>>>;
      template <flags::Flag Option>
      struct AtomLParRParTestListComp
          : seq<TestListComp<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (top_is<asdl::Comprehension>()) {
              std::vector<asdl::Comprehension> generators;
              generators.reserve(size() - 1);
              while (size() > 1) {
                generators.push_back(pop<asdl::Comprehension>());
              }
              outer.push(asdl::GeneratorExp{pop<asdl::ExprImpl>(),
                                            std::move(generators)});
            } else {
              asdl::Tuple tuple;
              tuple.elts.reserve(size());
              transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
              outer.push(std::move(tuple));
            }
          }
        };
      };
      struct EmptyTuple : success {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Tuple{});
          }
        };
      };
      template <flags::Flag Option>
      using AtomLParRParContent =
          sor<YieldExpr<flags::mask<Option, flags::IMPLICIT>>,
              AtomLParRParTest<flags::mask<Option, flags::IMPLICIT>>,
              AtomLParRParTestListComp<flags::mask<Option, flags::IMPLICIT>>,
              EmptyTuple>;
      template <flags::Flag Option>
      using AtomLParRPar =
          if_must<LPar<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                  flags::IMPLICIT>>,
                  AtomLParRParContent<flags::set<
                      flags::mask<Option, flags::IMPLICIT>, flags::IMPLICIT>>,
                  RPar<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct AtomLBrtRBrt
          : if_must<
                LBrt<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                flags::IMPLICIT>>,
                opt<TestListComp<flags::set<
                    flags::mask<Option, flags::IMPLICIT>, flags::IMPLICIT>>>,
                RBrt<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (top_is<asdl::Comprehension>()) {
              std::vector<asdl::Comprehension> generators;
              generators.reserve(size() - 1);
              while (size() > 1) {
                generators.push_back(pop<asdl::Comprehension>());
              }
              outer.push(
                  asdl::ListComp{pop<asdl::ExprImpl>(), std::move(generators)});
            } else {
              asdl::List list;
              list.elts.reserve(size());
              transform<asdl::ExprImpl>(std::back_inserter(list.elts));
              outer.push(std::move(list));
            }
          }
        };
      };
      template <flags::Flag Option>
      struct DictMaker;
      template <flags::Flag Option>
      struct SetMaker;
      struct EmptyDict : success {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Dict{});
          }
        };
      };
      template <flags::Flag Option>
      using DictOrSetMaker =
          sor<DictMaker<flags::mask<Option, flags::IMPLICIT>>,
              SetMaker<flags::mask<Option, flags::IMPLICIT>>, EmptyDict>;
      template <flags::Flag Option>
      using AtomLBrcRBrc =
          if_must<LBrc<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                  flags::IMPLICIT>>,
                  DictOrSetMaker<flags::set<
                      flags::mask<Option, flags::IMPLICIT>, flags::IMPLICIT>>,
                  RBrc<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using AtomName =
          must<minus<Name<flags::mask<Option, flags::IMPLICIT>>, Keywords>>;
      template <flags::Flag Option>
      struct Atom : sor<AtomLParRPar<flags::mask<Option, flags::IMPLICIT>>,
                        AtomLBrtRBrt<flags::mask<Option, flags::IMPLICIT>>,
                        AtomLBrcRBrc<flags::mask<Option, flags::IMPLICIT>>,
                        STRING<flags::mask<Option, flags::IMPLICIT>>,
                        NUMBER<flags::mask<Option, flags::IMPLICIT>>,
                        Ellipsis<flags::mask<Option, flags::IMPLICIT>>,
                        None<flags::mask<Option, flags::IMPLICIT>>,
                        True<flags::mask<Option, flags::IMPLICIT>>,
                        False<flags::mask<Option, flags::IMPLICIT>>,
                        AtomName<flags::mask<Option, flags::IMPLICIT>>> {};
      template <flags::Flag Option>
      struct SubscriptBound : opt<Test<Option>> {
        using Transform = rules::OptionalCapture<asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct SubscriptStop : opt<Colon<Option>, SubscriptBound<Option>> {
        using Transform = rules::OptionalCapture<std::optional<asdl::ExprImpl>>;
      };
      template <flags::Flag Option>
      struct SubscriptStartStopStep
          : seq<SubscriptBound<Option>, Colon<Option>, SubscriptBound<Option>,
                SubscriptStop<Option>> {
        using Transform =
            rules::ReshapeCapture<asdl::Slice, std::optional<asdl::ExprImpl>,
                                  std::optional<asdl::ExprImpl>,
                                  std::optional<asdl::ExprImpl>>;
      };
      template <flags::Flag Option>
      struct SubscriptIndex : seq<Test<Option>> {
        using Transform = rules::ReshapeCapture<asdl::Index, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct Subscript
          : sor<SubscriptStartStopStep<Option>, SubscriptIndex<Option>> {
        using Transform =
            rules::VariantCapture<asdl::ExtSlice, asdl::Index, asdl::Slice>;
      };
      template <flags::Flag Option>
      struct TestList : list_tail<Test<flags::mask<Option, flags::IMPLICIT>>,
                                  Comma<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 0) {
              asdl::Tuple tuple;
              tuple.elts.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
              outer.push(std::move(tuple));
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct SetMaker : CompOrCommaList<flags::mask<Option, flags::IMPLICIT>,
                                        TestOrStarExpr> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (top_is<asdl::Comprehension>()) {
              std::vector<asdl::Comprehension> generators;
              generators.reserve(size() - 1);
              while (size() > 1) {
                generators.push_back(pop<asdl::Comprehension>());
              }
              outer.push(
                  asdl::SetComp{pop<asdl::ExprImpl>(), std::move(generators)});
            } else {
              asdl::Set set;
              set.elts.reserve(size());
              transform<asdl::ExprImpl>(std::back_inserter(set.elts));
              outer.push(std::move(set));
            }
          }
        };
      };
      template <flags::Flag Option>
      struct DictMaker : CompOrCommaList<flags::mask<Option, flags::IMPLICIT>,
                                         DictMakerElt> {
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
              outer.push(asdl::DictComp{pop<asdl::ExprImpl>(), std::move(value),
                                        std::move(generators)});
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
              outer.push(std::move(dict));
            }
          }
        };
      };
      template <flags::Flag Option>
      struct ArgumentArg
          : seq<Test<flags::mask<Option, flags::IMPLICIT>>,
                opt<CompFor<flags::mask<Option, flags::IMPLICIT>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
              std::vector<asdl::Comprehension> generators;
              generators.reserve(size() - 1);
              while (size() > 1) {
                generators.push_back(pop<asdl::Comprehension>());
              }
              outer.push(asdl::GeneratorExp{pop<asdl::ExprImpl>(),
                                            std::move(generators)});
            } else {
              outer.push(pop<asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct ArgumentKeyword : seq<Name<flags::mask<Option, flags::IMPLICIT>>,
                                   Eq<flags::mask<Option, flags::IMPLICIT>>,
                                   Test<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Keyword keyword{{}, pop<asdl::ExprImpl>()};
            keyword.arg = pop<asdl::Name>();
            outer.push(std::move(keyword));
          }
        };
      };
      template <flags::Flag Option>
      struct ArgumentKeywordUnpack
          : if_must<Unpack<flags::mask<Option, flags::IMPLICIT>>,
                    Test<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Keyword{{}, pop<asdl::ExprImpl>()});
          }
        };
      };
      template <flags::Flag Option>
      struct ArgumentArgUnpack
          : if_must<StarOp<flags::mask<Option, flags::IMPLICIT>>,
                    Test<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::ReshapeCapture<asdl::Starred, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct Argument
          : sor<ArgumentKeywordUnpack<flags::mask<Option, flags::IMPLICIT>>,
                ArgumentArgUnpack<flags::mask<Option, flags::IMPLICIT>>,
                ArgumentKeyword<flags::mask<Option, flags::IMPLICIT>>,
                ArgumentArg<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::VariantCapture<asdl::ExprImpl, asdl::Keyword>;
      };
      template <flags::Flag Option>
      struct CompIf
          : if_must<If<flags::mask<Option, flags::IMPLICIT>>,
                    TestNocond<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.template top<asdl::Comprehension>().ifs.push_back(
                pop<asdl::ExprImpl>());
          }
        };
      };
      template <flags::Flag Option>
      struct YieldArgFrom
          : if_must<From<flags::mask<Option, flags::IMPLICIT>>,
                    Test<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform =
            rules::ReshapeCapture<asdl::YieldFrom, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct YieldArgTestList
          : seq<TestList<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::ReshapeCapture<asdl::Yield, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      using YieldArg =
          sor<YieldArgFrom<flags::mask<Option, flags::IMPLICIT>>,
              YieldArgTestList<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct YieldExpr
          : std::conditional_t<
                flags::get<Option, flags::ASYNC_FLOW, flags::SCOPE_FLOW>,
                seq<Yield<flags::mask<Option, flags::IMPLICIT>>,
                    opt<YieldArg<flags::mask<Option, flags::IMPLICIT>>>>,
                failure> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (has_value()) {
              outer.push(pop<asdl::ExprImpl>());
            } else {
              outer.push(asdl::Yield{});
            }
          }
        };
      };
    } // namespace grammar
  }   // namespace library
} // namespace chimera
