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

#include "asdl/asdl.hpp"
#include "grammar/exprfwd.hpp"
#include "grammar/flags.hpp"
#include "grammar/identifier.hpp"
#include "grammar/keys.hpp"
#include "grammar/number.hpp"
#include "grammar/oper.hpp"
#include "grammar/rules.hpp"
#include "grammar/string.hpp"

#include <algorithm>
#include <numeric>
#include <type_traits>

namespace chimera::library::grammar {
  template <flags::Flag Option>
  struct Atom;
  template <flags::Flag Option>
  struct Argument;
  template <flags::Flag Option>
  using ArgList = list_tail<Argument<Option>, Comma<Option>>;
  template <flags::Flag Option>
  struct TrailerLParRPar
      : if_must<LPar<flags::set<Option, flags::IMPLICIT>>,
                opt<ArgList<flags::set<Option, flags::IMPLICIT>>>,
                RPar<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl, asdl::Keyword> {
      template <typename Outer>
      void success(Outer &&outer) {
        asdl::Call call{outer.template pop<asdl::ExprImpl>(), {}, {}};
        call.keywords.reserve(size());
        call.args.reserve(size());
        while (has_value()) {
          if (top_is<asdl::Keyword>()) {
            call.keywords.emplace_back(pop<asdl::Keyword>());
          } else {
            call.args.emplace_back(pop<asdl::ExprImpl>());
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
  struct TrailerLBrtRBrt : sor<SubscriptSingle<Option>, SubscriptList<Option>> {
    struct Transform : rules::Stack<asdl::SliceImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 1) {
          asdl::ExtSlice ext_slice;
          transform<asdl::SliceImpl>(std::back_inserter(ext_slice.dims));
          outer.push(asdl::Subscript{outer.template pop<asdl::ExprImpl>(),
                                     asdl::SliceImpl{std::move(ext_slice)}});
        } else {
          outer.push(asdl::Subscript{outer.template pop<asdl::ExprImpl>(),
                                     pop<asdl::SliceImpl>()});
        }
      }
    };
  };
  template <flags::Flag Option>
  struct TrailerPeriod : if_must<Period<Option>, Name<Option>> {
    struct Transform : rules::Stack<asdl::Name> {
      template <typename Outer>
      void success(Outer &&outer) {
        outer.push(asdl::Attribute{outer.template pop<asdl::ExprImpl>(),
                                   pop<asdl::Name>()});
      }
    };
  };
  template <flags::Flag Option>
  using Trailer = sor<TrailerLParRPar<Option>, TrailerLBrtRBrt<Option>,
                      TrailerPeriod<Option>>;
  template <flags::Flag Option>
  using AtomExprAtomTrailer =
      seq<Atom<flags::mask<Option, flags::DISCARD, flags::IMPLICIT>>,
          star<Trailer<Option>>>;
  template <flags::Flag Option>
  struct AtomExprAwait
      : std::conditional_t<flags::get<Option, flags::ASYNC_FLOW>,
                           if_must<Await<Option>, AtomExprAtomTrailer<Option>>,
                           failure> {
    using Transform = rules::ReshapeCapture<asdl::Await, asdl::ExprImpl>;
  };
  template <flags::Flag Option>
  struct AtomExpr : sor<AtomExprAwait<Option>, AtomExprAtomTrailer<Option>> {};
  template <flags::Flag Option>
  struct Power;
  template <flags::Flag Option>
  struct Factor;
  template <flags::Flag Option>
  struct Unary : if_must<sor<UAdd<Option>, USub<Option>, BitNot<Option>>,
                         Factor<Option>> {
    using Transform =
        rules::ReshapeCapture<asdl::Unary, asdl::Unary::Op, asdl::ExprImpl>;
  };
  template <flags::Flag Option>
  struct Factor : sor<Unary<Option>, Power<Option>> {};
  template <flags::Flag Option>
  struct TermTail : if_must<sor<Mult<Option>, MatMult<Option>, Div<Option>,
                                Mod<Option>, FloorDiv<Option>>,
                            Factor<Option>> {
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
  using Term = seq<Factor<Option>, star<TermTail<Option>>>;
  template <flags::Flag Option>
  struct ArithExprTail : if_must<sor<Add<Option>, Sub<Option>>, Term<Option>> {
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
  using ArithExpr = seq<Term<Option>, star<ArithExprTail<Option>>>;
  template <flags::Flag Option>
  struct ShiftExprTail
      : if_must<sor<LShift<Option>, RShift<Option>>, ArithExpr<Option>> {
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
  struct ShiftExpr : seq<ArithExpr<Option>, star<ShiftExprTail<Option>>> {};
  template <flags::Flag Option>
  struct AndExpr : list_must<ShiftExpr<Option>, BitAnd<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 1) {
          asdl::Bin bin{asdl::Operator::BIT_AND, {}};
          bin.values.reserve(length);
          transform<asdl::ExprImpl>(std::back_inserter(bin.values));
          outer.push(std::move(bin));
        } else if (length == 1) {
          outer.push(pop<asdl::ExprImpl>());
        }
      }
    };
  };
  template <flags::Flag Option>
  struct XorExpr : list_must<AndExpr<Option>, BitXor<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 1) {
          asdl::Bin bin{asdl::Operator::BIT_XOR, {}};
          bin.values.reserve(length);
          transform<asdl::ExprImpl>(std::back_inserter(bin.values));
          outer.push(std::move(bin));
        } else if (length == 1) {
          outer.push(pop<asdl::ExprImpl>());
        }
      }
    };
  };
  template <flags::Flag Option>
  struct Expr : list_must<XorExpr<Option>, BitOr<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 1) {
          asdl::Bin bin{asdl::Operator::BIT_OR, {}};
          bin.values.reserve(length);
          transform<asdl::ExprImpl>(std::back_inserter(bin.values));
          outer.push(std::move(bin));
        } else if (length == 1) {
          outer.push(pop<asdl::ExprImpl>());
        }
      }
    };
  };
  template <flags::Flag Option>
  using CompOp = sor<Lt<Option>, Gt<Option>, EqEq<Option>, GtE<Option>,
                     LtE<Option>, NotEq<Option>, InOp<Option>, NotIn<Option>,
                     Is<Option>, IsNot<Option>>;
  template <flags::Flag Option>
  struct ComparisonTail : if_must<CompOp<Option>, Expr<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl, asdl::CompareExpr::Op> {
      template <typename Outer>
      void success(Outer &&outer) {
        auto expr = outer.template pop<asdl::ExprImpl>();
        if (expr.template update_if<asdl::Compare>([this](auto &&compare) {
              compare.comparators.emplace_back(
                  reshape<asdl::CompareExpr, asdl::CompareExpr::Op,
                          asdl::ExprImpl>());
              return compare;
            })) {
          outer.push(std::move(expr));
        } else {
          outer.push(
              asdl::Compare{std::move(expr),
                            {reshape<asdl::CompareExpr, asdl::CompareExpr::Op,
                                     asdl::ExprImpl>()}});
        }
      }
    };
  };
  template <flags::Flag Option>
  using Comparison = seq<Expr<Option>, star<ComparisonTail<Option>>>;
  template <flags::Flag Option>
  struct DictMakerUnpack : Unpack<Option> {
    struct Transform {
      template <typename Outer>
      void finalize(Transform & /*unused*/, Outer &&outer) {
        outer.push(asdl::UnpackDict{});
      }
    };
  };
  template <flags::Flag Option>
  struct NotTestNot
      : if_must<Not<Option>, sor<NotTestNot<Option>, Comparison<Option>>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        outer.push(asdl::Unary{asdl::Unary::NOT, pop<asdl::ExprImpl>()});
      }
    };
  };
  template <flags::Flag Option>
  using NotTest = sor<NotTestNot<Option>, Comparison<Option>>;
  template <flags::Flag Option>
  struct AndTest : list_must<NotTest<Option>, And<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 1) {
          asdl::Bool asdlBool{asdl::Bool::AND, {}};
          asdlBool.values.reserve(length);
          transform<asdl::ExprImpl>(std::back_inserter(asdlBool.values));
          outer.push(std::move(asdlBool));
        } else if (length == 1) {
          outer.push(pop<asdl::ExprImpl>());
        }
      }
    };
  };
  template <flags::Flag Option>
  struct OrTest : list_must<AndTest<Option>, Or<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 1) {
          asdl::Bool asdlBool{asdl::Bool::OR, {}};
          asdlBool.values.reserve(length);
          transform<asdl::ExprImpl>(std::back_inserter(asdlBool.values));
          outer.push(std::move(asdlBool));
        } else if (length == 1) {
          outer.push(pop<asdl::ExprImpl>());
        }
      }
    };
  };
  template <flags::Flag Option>
  struct LambDef;
  template <flags::Flag Option>
  using Test = sor<ConditionalExpression<Option>, LambDef<Option>>;
  template <flags::Flag Option>
  struct ConditionalExpression
      : seq<OrTest<Option>,
            opt_must<If<Option>, OrTest<Option>, Else<Option>, Test<Option>>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 1) {
          outer.push(reshape<asdl::IfExp, asdl::ExprImpl, asdl::ExprImpl,
                             asdl::ExprImpl>());
        } else if (length == 1) {
          outer.push(pop<asdl::ExprImpl>());
        }
      }
    };
  };
  template <flags::Flag Option>
  struct VarargsList;
  template <flags::Flag Option>
  struct LambDef
      : if_must<Lambda<Option>, opt<VarargsList<Option>>, Colon<Option>,
                Test<flags::set<flags::unSet<Option, flags::ASYNC_FLOW>,
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
  struct DictMakerEltStart : seq<Test<Option>, Colon<Option>> {
    using Transform = rules::VariantCapture<asdl::ExprImpl>;
  };
  template <flags::Flag Option>
  using DictMakerElt = sor<if_must<DictMakerUnpack<Option>, Expr<Option>>,
                           if_must<DictMakerEltStart<Option>, Test<Option>>>;
  template <flags::Flag Option>
  struct StarExpr : seq<StarOp<Option>, Expr<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        outer.push(asdl::Starred{pop<asdl::ExprImpl>()});
      }
    };
  };
  template <flags::Flag Option>
  struct ExprList
      : list_tail<sor<Expr<Option>, StarExpr<Option>>, Comma<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 0) {
          asdl::Tuple tuple;
          tuple.elts.reserve(length);
          transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
          outer.push(std::move(tuple));
        } else if (length == 1) {
          outer.push(pop<asdl::ExprImpl>());
        }
      }
    };
  };
  template <flags::Flag Option>
  struct Power : seq<AtomExpr<Option>, opt_must<Pow<Option>, Factor<Option>>> {
    struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
      template <typename Outer>
      void success(Outer &&outer) {
        auto length = size();
        if (length == 3) {
          asdl::Bin bin;
          bin.values.reserve(2);
          {
            auto expr = pop<asdl::ExprImpl>();
            bin.op = pop<asdl::Operator>();
            bin.values.emplace_back(pop<asdl::ExprImpl>());
            bin.values.emplace_back(std::move(expr));
          }
          outer.push(std::move(bin));
        } else if (length == 1) {
          outer.push(pop<asdl::ExprImpl>());
        } else {
          outer.push(pop<asdl::ExprImpl>());
          clear(); // TODO(asakatida): error
        }
      }
    };
  };
  template <flags::Flag Option>
  struct CompForAsync
      : std::conditional_t<flags::get<Option, flags::ASYNC_FLOW>,
                           if_must<Async<Option>, For<Option>, ExprList<Option>,
                                   In<Option>, OrTest<Option>>,
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
      : if_must<For<Option>, ExprList<Option>, In<Option>, OrTest<Option>> {
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
      seq<sor<CompForAsync<Option>, CompForSync<Option>>,
          star<sor<CompForAsync<Option>, CompForSync<Option>, CompIf<Option>>>>;
  template <flags::Flag Option, template <flags::Flag> typename Elt>
  using CompOrCommaList = sor<seq<Elt<Option>, CompFor<Option>>,
                              list_tail<Elt<Option>, Comma<Option>>>;
  template <flags::Flag Option>
  struct VarargsListNameEqTest
      : seq<Name<Option>, opt_must<Eq<Option>, Test<Option>>> {
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
              .emplace_back(std::move(arg));
        }(outer.template top<asdl::Arguments>());
      }
    };
  };
  template <flags::Flag Option>
  struct VarargsListStarName : seq<StarOp<Option>, opt<Name<Option>>> {
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
      : seq<Unpack<Option>, Name<Option>, opt<Comma<Option>>> {
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
                VarargsListNameEqTest<Option>,
                star<Comma<Option>, VarargsListNameEqTest<Option>>,
                opt<Comma<Option>,
                    opt<sor<if_must<VarargsListStarName<Option>,
                                    star<Comma<Option>,
                                         VarargsListNameEqTest<Option>>,
                                    opt<Comma<Option>,
                                        opt<VarargsListUnpackName<Option>>>>,
                            VarargsListUnpackName<Option>>>>>,
            if_must<VarargsListStarName<Option>,
                    star<Comma<Option>, VarargsListNameEqTest<Option>>,
                    opt<Comma<Option>, opt<VarargsListUnpackName<Option>>>>,
            VarargsListUnpackName<Option>> {};
  template <flags::Flag Option>
  using TestOrStarExpr = sor<Test<Option>, StarExpr<Option>>;
  template <flags::Flag Option>
  using TestListComp = CompOrCommaList<Option, TestOrStarExpr>;
  template <flags::Flag Option>
  using TestListStarExpr = list_tail<TestOrStarExpr<Option>, Comma<Option>>;
  template <flags::Flag Option>
  struct ExprList;
  template <flags::Flag Option>
  struct LambDefNoCond;
  template <flags::Flag Option>
  struct TestNocond : sor<OrTest<Option>, LambDefNoCond<Option>> {};
  template <flags::Flag Option>
  struct LambDefNoCond
      : if_must<Lambda<Option>, opt<VarargsList<Option>>, Colon<Option>,
                TestNocond<flags::set<flags::unSet<Option, flags::ASYNC_FLOW>,
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
  using AtomLParRParTest = seq<Test<Option>, at<RPar<Option>>>;
  template <flags::Flag Option>
  struct AtomLParRParTestListComp : seq<TestListComp<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (top_is<asdl::Comprehension>()) {
          std::vector<asdl::Comprehension> generators;
          generators.reserve(size() - 1);
          while (size() > 1) {
            generators.emplace_back(pop<asdl::Comprehension>());
          }
          outer.push(
              asdl::GeneratorExp{pop<asdl::ExprImpl>(), std::move(generators)});
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
      void finalize(Transform & /*unused*/, Outer &&outer) {
        outer.push(asdl::Tuple{});
      }
    };
  };
  template <flags::Flag Option>
  using AtomLParRParContent = sor<YieldExpr<Option>, AtomLParRParTest<Option>,
                                  AtomLParRParTestListComp<Option>, EmptyTuple>;
  template <flags::Flag Option>
  using AtomLParRPar =
      if_must<LPar<flags::set<Option, flags::IMPLICIT>>,
              AtomLParRParContent<flags::set<Option, flags::IMPLICIT>>,
              RPar<Option>>;
  template <flags::Flag Option>
  struct AtomLBrtRBrt
      : if_must<LBrt<flags::set<Option, flags::IMPLICIT>>,
                opt<TestListComp<flags::set<Option, flags::IMPLICIT>>>,
                RBrt<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (top_is<asdl::Comprehension>()) {
          std::vector<asdl::Comprehension> generators;
          generators.reserve(size() - 1);
          while (size() > 1) {
            generators.emplace_back(pop<asdl::Comprehension>());
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
      void finalize(Transform & /*unused*/, Outer &&outer) {
        outer.push(asdl::Dict{});
      }
    };
  };
  template <flags::Flag Option>
  using DictOrSetMaker = sor<DictMaker<Option>, SetMaker<Option>, EmptyDict>;
  template <flags::Flag Option>
  using AtomLBrcRBrc =
      if_must<LBrc<flags::set<Option, flags::IMPLICIT>>,
              DictOrSetMaker<flags::set<Option, flags::IMPLICIT>>,
              RBrc<Option>>;
  template <flags::Flag Option>
  using AtomName = minus<Name<Option>, Keywords>;
  template <flags::Flag Option>
  struct AtomToken
      : sor<STRING<Option>, NUMBER<Option>, Ellipsis<Option>, None<Option>,
            True<Option>, False<Option>, AtomName<Option>> {};
  template <flags::Flag Option>
  struct Atom
      : sor<AtomLParRPar<Option>, AtomLBrtRBrt<Option>, AtomLBrcRBrc<Option>,
            AtomToken<flags::mask<Option, flags::DISCARD, flags::IMPLICIT>>> {};
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
  struct TestList : list_tail<Test<Option>, Comma<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 0) {
          asdl::Tuple tuple;
          tuple.elts.reserve(length);
          transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
          outer.push(std::move(tuple));
        } else {
          outer.push(pop<asdl::ExprImpl>());
        }
      }
    };
  };
  template <flags::Flag Option>
  struct SetMaker : CompOrCommaList<Option, TestOrStarExpr> {
    struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (top_is<asdl::Comprehension>()) {
          std::vector<asdl::Comprehension> generators;
          generators.reserve(size() - 1);
          while (size() > 1) {
            generators.emplace_back(pop<asdl::Comprehension>());
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
  struct DictMaker : CompOrCommaList<Option, DictMakerElt> {
    struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (top_is<asdl::Comprehension>()) {
          std::vector<asdl::Comprehension> generators;
          generators.reserve(size() - 1);
          while (size() > 1) {
            generators.emplace_back(pop<asdl::Comprehension>());
          }
          auto value = pop<asdl::ExprImpl>();
          outer.push(asdl::DictComp{pop<asdl::ExprImpl>(), std::move(value),
                                    std::move(generators)});
        } else {
          asdl::Dict dict;
          auto length = size() / 2;
          dict.keys.reserve(length);
          dict.values.reserve(length);
          while (has_value()) {
            dict.values.emplace_back(pop<asdl::ExprImpl>());
            dict.keys.emplace_back(pop<asdl::ExprImpl>());
          }
          std::reverse(dict.values.begin(), dict.values.end());
          std::reverse(dict.keys.begin(), dict.keys.end());
          outer.push(std::move(dict));
        }
      }
    };
  };
  template <flags::Flag Option>
  struct ArgumentArg : seq<Test<Option>, opt<CompFor<Option>>> {
    struct Transform : rules::Stack<asdl::ExprImpl, asdl::Comprehension> {
      template <typename Outer>
      void success(Outer &&outer) {
        if (auto length = size(); length > 1) {
          std::vector<asdl::Comprehension> generators;
          generators.reserve(length - 1);
          while (size() > 1) {
            generators.emplace_back(pop<asdl::Comprehension>());
          }
          outer.push(
              asdl::GeneratorExp{pop<asdl::ExprImpl>(), std::move(generators)});
        } else {
          outer.push(pop<asdl::ExprImpl>());
        }
      }
    };
  };
  template <flags::Flag Option>
  struct ArgumentKeyword : seq<Name<Option>, Eq<Option>, Test<Option>> {
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
  struct ArgumentKeywordUnpack : if_must<Unpack<Option>, Test<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        outer.push(asdl::Keyword{{}, pop<asdl::ExprImpl>()});
      }
    };
  };
  template <flags::Flag Option>
  struct ArgumentArgUnpack : if_must<StarOp<Option>, Test<Option>> {
    using Transform = rules::ReshapeCapture<asdl::Starred, asdl::ExprImpl>;
  };
  template <flags::Flag Option>
  struct Argument
      : sor<ArgumentKeywordUnpack<Option>, ArgumentArgUnpack<Option>,
            ArgumentKeyword<Option>, ArgumentArg<Option>> {
    using Transform = rules::VariantCapture<asdl::ExprImpl, asdl::Keyword>;
  };
  template <flags::Flag Option>
  struct CompIf : if_must<If<Option>, TestNocond<Option>> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Outer>
      void success(Outer &&outer) {
        outer.template top<asdl::Comprehension>().ifs.emplace_back(
            pop<asdl::ExprImpl>());
      }
    };
  };
  template <flags::Flag Option>
  struct YieldArgFrom : if_must<From<Option>, Test<Option>> {
    using Transform = rules::ReshapeCapture<asdl::YieldFrom, asdl::ExprImpl>;
  };
  template <flags::Flag Option>
  struct YieldArgTestList : seq<TestList<Option>> {
    using Transform = rules::ReshapeCapture<asdl::Yield, asdl::ExprImpl>;
  };
  template <flags::Flag Option>
  using YieldArg = sor<YieldArgFrom<Option>, YieldArgTestList<Option>>;
  template <flags::Flag Option>
  struct YieldExpr
      : std::conditional_t<
            flags::get<Option, flags::ASYNC_FLOW, flags::SCOPE_FLOW>,
            seq<Yield<Option>, opt<YieldArg<Option>>>, failure> {
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
} // namespace chimera::library::grammar
