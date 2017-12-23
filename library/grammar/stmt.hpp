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

//! parse definitions for top level code statements.

#pragma once

#include <algorithm>
#include <numeric>
#include <type_traits>

#include <tao/pegtl.hpp>

#include "grammar/control.hpp"
#include "grammar/expr.hpp"
#include "grammar/keys.hpp"
#include "grammar/oper.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TFPDef
          : tao::pegtl::seq<
                Name<Implicit>,
                tao::pegtl::opt<
                    Colon<Implicit>,
                    tao::pegtl::must<Test<Implicit, AsyncFlow, ScopeFlow>>>> {};
      struct TFPDefAction : Stack<asdl::ExprImpl, asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Arg arg;
          if (top_is<asdl::ExprImpl>()) {
            arg.annotation = pop<asdl::ExprImpl>();
          }
          arg.name = pop<asdl::Name>();
          outer.push(std::move(arg));
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TFPDef<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<TFPDef<Implicit, AsyncFlow, ScopeFlow>, TFPDefAction> {
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TypedargsListTFPDefEqTest
          : tao::pegtl::seq<
                TFPDef<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::opt<
                    Eq<Implicit>,
                    tao::pegtl::must<Test<Implicit, AsyncFlow, ScopeFlow>>>> {};
      struct TypedargsListTFPDefEqTestAction
          : Stack<asdl::ExprImpl, asdl::Arg> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (top_is<asdl::ExprImpl>()) {
            auto argDefault = pop<asdl::ExprImpl>();
            top<asdl::Arg>().arg_default = std::move(argDefault);
          }
          if (!outer.has_value()) {
            outer.push(asdl::Arguments{});
          }
          if (outer.template top<asdl::Arguments>().vararg) {
            outer.template top<asdl::Arguments>().kwonlyargs.push_back(
                pop<asdl::Arg>());
          } else {
            outer.template top<asdl::Arguments>().args.push_back(
                pop<asdl::Arg>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TypedargsListTFPDefEqTest<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<
                TypedargsListTFPDefEqTest<Implicit, AsyncFlow, ScopeFlow>,
                TypedargsListTFPDefEqTestAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TypedargsListStarTFPDef
          : tao::pegtl::seq<
                Star<Implicit>,
                tao::pegtl::opt<TFPDef<Implicit, AsyncFlow, ScopeFlow>>> {};
      struct TypedargsListStarTFPDefAction : Stack<asdl::Arg> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (outer.has_value()) {
            outer.template top<asdl::Arguments>().vararg =
                has_value() ? pop<asdl::Arg>() : asdl::Arg{};
          } else {
            outer.push(asdl::Arguments{
                {}, has_value() ? pop<asdl::Arg>() : asdl::Arg{}, {}, {}});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TypedargsListStarTFPDef<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<TypedargsListStarTFPDef<Implicit, AsyncFlow, ScopeFlow>,
                        TypedargsListStarTFPDefAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TypedargsListUnpackTfpdef
          : tao::pegtl::if_must<Unpack<Implicit>,
                                TFPDef<Implicit, AsyncFlow, ScopeFlow>,
                                tao::pegtl::opt<Comma<Implicit>>> {};
      struct TypedargsListUnpackTfpdefAction : Stack<asdl::Arg> {
        template <typename Outer>
        void success(Outer &&outer) {
          auto arg = pop<asdl::Arg>();
          if (outer.has_value()) {
            outer.template top<asdl::Arguments>().kwarg = std::move(arg);
          } else {
            outer.push(asdl::Arguments{{}, {}, {}, std::move(arg)});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TypedargsListUnpackTfpdef<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<
                TypedargsListUnpackTfpdef<Implicit, AsyncFlow, ScopeFlow>,
                TypedargsListUnpackTfpdefAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TypedArgsList
          : tao::pegtl::sor<
                tao::pegtl::seq<
                    TypedargsListTFPDefEqTest<Implicit, AsyncFlow, ScopeFlow>,
                    tao::pegtl::star<Comma<Implicit>,
                                     TypedargsListTFPDefEqTest<
                                         Implicit, AsyncFlow, ScopeFlow>>,
                    tao::pegtl::opt<
                        Comma<Implicit>,
                        tao::pegtl::opt<tao::pegtl::sor<
                            tao::pegtl::seq<
                                TypedargsListStarTFPDef<Implicit, AsyncFlow,
                                                        ScopeFlow>,
                                tao::pegtl::star<
                                    Comma<Implicit>,
                                    TypedargsListTFPDefEqTest<
                                        Implicit, AsyncFlow, ScopeFlow>>,
                                tao::pegtl::opt<
                                    Comma<Implicit>,
                                    tao::pegtl::opt<TypedargsListUnpackTfpdef<
                                        Implicit, AsyncFlow, ScopeFlow>>>>,
                            TypedargsListUnpackTfpdef<Implicit, AsyncFlow,
                                                      ScopeFlow>>>>>,
                tao::pegtl::seq<
                    TypedargsListStarTFPDef<Implicit, AsyncFlow, ScopeFlow>,
                    tao::pegtl::star<Comma<Implicit>,
                                     TypedargsListTFPDefEqTest<
                                         Implicit, AsyncFlow, ScopeFlow>>,
                    tao::pegtl::opt<Comma<Implicit>,
                                    tao::pegtl::opt<TypedargsListUnpackTfpdef<
                                        Implicit, AsyncFlow, ScopeFlow>>>>,
                TypedargsListUnpackTfpdef<Implicit, AsyncFlow, ScopeFlow>> {};
      struct TypedArgsListAction : Stack<asdl::Arguments> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(pop<asdl::Arguments>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<TypedArgsList<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<TypedArgsList<Implicit, AsyncFlow, ScopeFlow>,
                        TypedArgsListAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Parameters
          : ParenOpt<Implicit, AsyncFlow, ScopeFlow, TypedArgsList> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SuiteWithDoc;
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Suite;
      template <bool Implicit, bool AsyncFlow>
      struct FuncDef
          : tao::pegtl::if_must<
                Def<Implicit>, Name<Implicit>,
                Parameters<Implicit, false, false>,
                tao::pegtl::opt<Arr<Implicit>,
                                tao::pegtl::must<Test<Implicit, false, false>>>,
                SuiteWithDoc<Implicit, AsyncFlow, false, true, false>> {};
      struct FuncDefAction
          : Stack<asdl::DocString, std::vector<asdl::StmtImpl>, asdl::ExprImpl,
                  asdl::Arguments, asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          // TODO reshape
          asdl::FunctionDef functionDef;
          functionDef.body = pop<std::vector<asdl::StmtImpl>>();
          if (top_is<asdl::DocString>()) {
            functionDef.doc_string = pop<asdl::DocString>();
          }
          if (top_is<asdl::ExprImpl>()) {
            functionDef.returns = pop<asdl::ExprImpl>();
          }
          if (top_is<asdl::Arguments>()) {
            functionDef.args = pop<asdl::Arguments>();
          }
          functionDef.name = pop<asdl::Name>();
          outer.push(asdl::StmtImpl{std::move(functionDef)});
        }
      };
      template <bool Implicit, bool AsyncFlow>
      struct Control<FuncDef<Implicit, AsyncFlow>>
          : ChangeState<FuncDef<Implicit, AsyncFlow>, FuncDefAction> {};
      template <bool Implicit>
      struct AsyncFuncDef
          : tao::pegtl::seq<Async<Implicit>, FuncDef<Implicit, true>> {};
      struct AsyncFuncDefAction : Stack<asdl::StmtImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          auto functionDef = std::get<asdl::FunctionDef>(
              std::move(*pop<asdl::StmtImpl>().value));
          outer.push(asdl::StmtImpl{asdl::AsyncFunctionDef{
              std::move(functionDef.name), functionDef.doc_string,
              std::move(functionDef.args), std::move(functionDef.body),
              std::move(functionDef.decorator_list),
              std::move(functionDef.returns)}});
        }
      };
      template <bool Implicit>
      struct Control<AsyncFuncDef<Implicit>>
          : ChangeState<AsyncFuncDef<Implicit>, AsyncFuncDefAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmtTarget
          : tao::pegtl::seq<TestListStarExpr<Implicit, AsyncFlow, ScopeFlow>> {
      };
      struct ExprStmtTargetAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (auto s = size(); s > 1) {
            asdl::Tuple tuple;
            tuple.elts.reserve(s);
            transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
            outer.push(asdl::ExprImpl{std::move(tuple)});
          } else {
            outer.push(pop<asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>,
                        ExprStmtTargetAction> {};
      template <bool Implicit>
      struct AugAssign
          : tao::pegtl::sor<
                AugAdd<Implicit>, AugSub<Implicit>, AugMult<Implicit>,
                AugMatMult<Implicit>, AugDiv<Implicit>, AugMod<Implicit>,
                AugBitAnd<Implicit>, AugBitOr<Implicit>, AugBitXor<Implicit>,
                AugLShift<Implicit>, AugRShift<Implicit>, AugPow<Implicit>,
                AugFloorDiv<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmtAugAssign
          : tao::pegtl::if_must<
                tao::pegtl::seq<ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>,
                                AugAssign<Implicit>>,
                tao::pegtl::sor<YieldExpr<Implicit, AsyncFlow, ScopeFlow>,
                                TestList<Implicit, AsyncFlow, ScopeFlow>>> {};
      struct ExprStmtAugAssignAction : Stack<asdl::ExprImpl, asdl::Operator> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::StmtImpl{reshape<asdl::AugAssign, asdl::ExprImpl,
                                            asdl::Operator, asdl::ExprImpl>()});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ExprStmtAugAssign<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ExprStmtAugAssign<Implicit, AsyncFlow, ScopeFlow>,
                        ExprStmtAugAssignAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmtAssign
          : tao::pegtl::seq<
                ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>, Eq<Implicit>,
                tao::pegtl::list_must<
                    tao::pegtl::sor<
                        YieldExpr<Implicit, AsyncFlow, ScopeFlow>,
                        ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>>,
                    Eq<Implicit>>> {};
      struct ExprStmtAssignAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Assign assign{{}, pop<asdl::ExprImpl>()};
          assign.targets.reserve(size());
          transform<asdl::ExprImpl>(std::back_inserter(assign.targets));
          outer.push(asdl::StmtImpl{std::move(assign)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ExprStmtAssign<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ExprStmtAssign<Implicit, AsyncFlow, ScopeFlow>,
                        ExprStmtAssignAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AnnAssign
          : tao::pegtl::if_must<
                tao::pegtl::seq<ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>,
                                Colon<Implicit>>,
                Test<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::opt<
                    Eq<Implicit>,
                    tao::pegtl::must<Test<Implicit, AsyncFlow, ScopeFlow>>>> {};
      struct AnnAssignAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (size() > 2) {
            // TODO reshape
            // outer.push(
            //     asdl::StmtImpl{reshape<asdl::AnnAssign, asdl::ExprImpl,
            //                            asdl::ExprImpl, asdl::ExprImpl>()});
          } else {
            outer.push(asdl::StmtImpl{
                reshape<asdl::AnnAssign, asdl::ExprImpl, asdl::ExprImpl>()});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<AnnAssign<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<AnnAssign<Implicit, AsyncFlow, ScopeFlow>,
                        AnnAssignAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmtExprStmtTarget
          : tao::pegtl::seq<ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>> {};
      struct ExprStmtExprStmtTargetAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::StmtImpl{asdl::Expr{pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ExprStmtExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ExprStmtExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>,
                        ExprStmtExprStmtTargetAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmt
          : tao::pegtl::sor<
                AnnAssign<Implicit, AsyncFlow, ScopeFlow>,
                ExprStmtAugAssign<Implicit, AsyncFlow, ScopeFlow>,
                ExprStmtAssign<Implicit, AsyncFlow, ScopeFlow>,
                ExprStmtExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DelStmt
          : tao::pegtl::if_must<Del<Implicit>,
                                ExprList<Implicit, AsyncFlow, ScopeFlow>> {};
      struct DelStmtAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Delete asdlDelete;
          asdlDelete.targets.reserve(size());
          transform<asdl::ExprImpl>(std::back_inserter(asdlDelete.targets));
          outer.push(asdl::StmtImpl{std::move(asdlDelete)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<DelStmt<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<DelStmt<Implicit, AsyncFlow, ScopeFlow>,
                        DelStmtAction> {};
      template <bool Implicit, bool LoopFlow>
      struct BreakStmt
          : std::conditional_t<LoopFlow, Break<Implicit>, tao::pegtl::failure> {
      };
      struct BreakStmtAction {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::StmtImpl{asdl::Break{}});
        }
      };
      template <bool Implicit, bool LoopFlow>
      struct Control<BreakStmt<Implicit, LoopFlow>>
          : ChangeState<BreakStmt<Implicit, LoopFlow>, BreakStmtAction> {};
      template <bool Implicit, bool LoopFlow>
      struct ContinueStmt : std::conditional_t<LoopFlow, Continue<Implicit>,
                                               tao::pegtl::failure> {};
      struct ContinueStmtAction {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::StmtImpl{asdl::Continue{}});
        }
      };
      template <bool Implicit, bool LoopFlow>
      struct Control<ContinueStmt<Implicit, LoopFlow>>
          : ChangeState<ContinueStmt<Implicit, LoopFlow>, ContinueStmtAction> {
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ReturnStmt
          : std::conditional_t<
                ScopeFlow,
                tao::pegtl::seq<
                    Return<Implicit>,
                    tao::pegtl::opt<TestList<Implicit, AsyncFlow, ScopeFlow>>>,
                tao::pegtl::failure> {};
      struct ReturnStmtAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (has_value()) {
            outer.push(asdl::StmtImpl{asdl::Return{pop<asdl::ExprImpl>()}});
          } else {
            outer.push(asdl::StmtImpl{asdl::Return{}});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ReturnStmt<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ReturnStmt<Implicit, AsyncFlow, ScopeFlow>,
                        ReturnStmtAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldStmt
          : tao::pegtl::seq<YieldExpr<Implicit, AsyncFlow, ScopeFlow>> {};
      struct YieldStmtAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::StmtImpl{asdl::Expr{pop<asdl::ExprImpl>()}});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<YieldStmt<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<YieldStmt<Implicit, AsyncFlow, ScopeFlow>,
                        YieldStmtAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct RaiseStmt
          : tao::pegtl::seq<
                Raise<Implicit>,
                tao::pegtl::opt<
                    Test<Implicit, AsyncFlow, ScopeFlow>,
                    tao::pegtl::opt<From<Implicit>,
                                    Test<Implicit, AsyncFlow, ScopeFlow>>>> {};
      struct RaiseStmtAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Raise raise;
          if (size() > 1) {
            raise.cause = pop<asdl::ExprImpl>();
          }
          if (has_value()) {
            raise.exc = pop<asdl::ExprImpl>();
          }
          outer.push(asdl::StmtImpl{std::move(raise)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<RaiseStmt<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<RaiseStmt<Implicit, AsyncFlow, ScopeFlow>,
                        RaiseStmtAction> {};
      template <bool Implicit>
      struct DottedName : tao::pegtl::list<Name<Implicit>, Period<Implicit>> {};
      struct DottedNameAction : Stack<asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          std::vector<asdl::Name> identifiers;
          identifiers.reserve(size());
          transform<asdl::Name>(std::back_inserter(identifiers));
          outer.push(std::reduce(identifiers.begin(), identifiers.end(),
                                 asdl::Name{},
                                 [](const auto &ida, const auto &idb) {
                                   auto id = ida.value;
                                   id.reserve(id.size() + idb.value.size() + 1);
                                   if (!id.empty()) {
                                     id.append(1, '.');
                                   }
                                   id.append(idb.value);
                                   return asdl::Name{id};
                                 }));
        }
      };
      template <bool Implicit>
      struct Control<DottedName<Implicit>>
          : ChangeState<DottedName<Implicit>, DottedNameAction> {};
      template <bool Implicit>
      struct DottedAsName
          : tao::pegtl::seq<DottedName<Implicit>,
                            tao::pegtl::opt<As<Implicit>, Name<Implicit>>> {};
      struct DottedAsNameAction : Stack<asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Alias alias;
          if (size() > 1) {
            alias.asname = pop<asdl::Name>();
          }
          alias.name = pop<asdl::Name>();
          outer.push(std::move(alias));
        }
      };
      template <bool Implicit>
      struct Control<DottedAsName<Implicit>>
          : ChangeState<DottedAsName<Implicit>, DottedAsNameAction> {};
      template <bool Implicit>
      struct DottedAsNames
          : tao::pegtl::list<DottedAsName<Implicit>, Comma<Implicit>> {};
      template <bool Implicit>
      struct ImportName
          : tao::pegtl::seq<Import<Implicit>, DottedAsNames<Implicit>> {};
      struct ImportNameAction : Stack<asdl::Alias> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Import import;
          import.names.reserve(size());
          transform<asdl::Alias>(std::back_inserter(import.names));
          outer.push(asdl::StmtImpl{std::move(import)});
        }
      };
      template <bool Implicit>
      struct Control<ImportName<Implicit>>
          : ChangeState<ImportName<Implicit>, ImportNameAction> {};
      template <bool Implicit>
      struct ImportFromPeriod : tao::pegtl::seq<PeriodImpl<Implicit>> {};
      struct ImportFromPeriodAction {
        template <typename Outer>
        void success(Outer &&outer) {
          std::get<asdl::ImportFrom>(
              *outer.template top<asdl::StmtImpl>().value)
              .module.value.append(1, '.');
        }
      };
      template <bool Implicit>
      struct Control<ImportFromPeriod<Implicit>>
          : ChangeState<ImportFromPeriod<Implicit>, ImportFromPeriodAction> {};
      template <bool Implicit>
      struct ImportFromModuleName
          : tao::pegtl::seq<tao::pegtl::not_at<Import<Implicit>>,
                            DottedName<Implicit>> {};
      struct ImportFromModuleNameAction : Stack<asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          std::get<asdl::ImportFrom>(
              *outer.template top<asdl::StmtImpl>().value)
              .module.value.append(pop<asdl::Name>().value);
        }
      };
      template <bool Implicit>
      struct Control<ImportFromModuleName<Implicit>>
          : ChangeState<ImportFromModuleName<Implicit>,
                        ImportFromModuleNameAction> {};
      template <bool Implicit>
      struct ImportAsName
          : tao::pegtl::seq<Name<Implicit>,
                            tao::pegtl::opt<As<Implicit>, Name<Implicit>>> {};
      struct ImportAsNameAction : Stack<asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Alias alias;
          if (size() > 1) {
            alias.asname = pop<asdl::Name>();
          }
          alias.name = pop<asdl::Name>();
          outer.push(std::move(alias));
        }
      };
      template <bool Implicit>
      struct Control<ImportAsName<Implicit>>
          : ChangeState<ImportAsName<Implicit>, ImportAsNameAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ImportAsNames
          : tao::pegtl::list_tail<ImportAsName<Implicit>, Comma<Implicit>> {};
      struct ImportAsNamesAction : Stack<asdl::Alias> {
        template <typename Outer>
        void success(Outer &&outer) {
          [this](auto &&importFrom) {
            importFrom.names.reserve(this->size());
            this->template transform<asdl::Alias>(
                std::back_inserter(importFrom.names));
          }(std::get<asdl::ImportFrom>(
              *outer.template top<asdl::StmtImpl>().value));
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<ImportAsNames<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<ImportAsNames<Implicit, AsyncFlow, ScopeFlow>,
                        ImportAsNamesAction> {};
      template <bool Implicit, bool ImportAll>
      struct ImportFrom
          : tao::pegtl::if_must<
                From<Implicit>,
                tao::pegtl::sor<tao::pegtl::seq<tao::pegtl::star<
                                                    ImportFromPeriod<Implicit>>,
                                                ImportFromModuleName<Implicit>>,
                                tao::pegtl::plus<PeriodImpl<Implicit>>>,
                Import<Implicit>,
                tao::pegtl::sor<std::conditional_t<ImportAll, Star<Implicit>,
                                                   tao::pegtl::failure>,
                                Paren<Implicit, false, false, ImportAsNames>,
                                ImportAsNames<Implicit, false, false>>> {};
      struct ImportFromAction : Stack<asdl::StmtImpl> {
        ImportFromAction() { push(asdl::StmtImpl{asdl::ImportFrom{}}); }
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(pop<asdl::StmtImpl>());
        }
      };
      template <bool Implicit, bool ImportAll>
      struct Control<ImportFrom<Implicit, ImportAll>>
          : ChangeState<ImportFrom<Implicit, ImportAll>, ImportFromAction> {};
      template <bool Implicit>
      struct GlobalStmt
          : tao::pegtl::if_must<
                Global<Implicit>,
                tao::pegtl::list_must<Name<Implicit>, Comma<Implicit>>> {};
      struct GlobalStmtAction : Stack<asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Global global;
          global.names.reserve(size());
          transform<asdl::Name>(std::back_inserter(global.names));
          outer.push(asdl::StmtImpl{std::move(global)});
        }
      };
      template <bool Implicit>
      struct Control<GlobalStmt<Implicit>>
          : ChangeState<GlobalStmt<Implicit>, GlobalStmtAction> {};
      template <bool Implicit>
      struct NonLocalStmt
          : tao::pegtl::if_must<
                Nonlocal<Implicit>,
                tao::pegtl::list_must<Name<Implicit>, Comma<Implicit>>> {};
      struct NonLocalStmtAction : Stack<asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::Nonlocal nonlocal;
          nonlocal.names.reserve(size());
          transform<asdl::Name>(std::back_inserter(nonlocal.names));
          outer.push(asdl::StmtImpl{std::move(nonlocal)});
        }
      };
      template <bool Implicit>
      struct Control<NonLocalStmt<Implicit>>
          : ChangeState<NonLocalStmt<Implicit>, NonLocalStmtAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AssertStmt
          : tao::pegtl::if_must<
                Assert<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::opt<Comma<Implicit>,
                                Test<Implicit, AsyncFlow, ScopeFlow>>> {};
      struct AssertStmtAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (size() > 1) {
            // TODO reshape
            // outer.push(asdl::StmtImpl{
            //     reshape<asdl::Assert, asdl::ExprImpl, asdl::ExprImpl>()});
          } else {
            outer.push(asdl::StmtImpl{reshape<asdl::Assert, asdl::ExprImpl>()});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<AssertStmt<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<AssertStmt<Implicit, AsyncFlow, ScopeFlow>,
                        AssertStmtAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtBranch
          : tao::pegtl::seq<
                Test<Implicit, AsyncFlow, ScopeFlow>,
                Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {};
      struct IfStmtBranchAction
          : Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer && /*outer*/) {
          // TODO reshape
          // outer.push(reshape<asdl::IfBranch, asdl::ExprImpl,
          //                    std::vector<asdl::StmtImpl>>());
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          IfStmtBranch<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<IfStmtBranch<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                     ImportAll>,
                        IfStmtBranchAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtIf
          : tao::pegtl::if_must<If<Implicit>,
                                IfStmtBranch<Implicit, AsyncFlow, LoopFlow,
                                             ScopeFlow, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtElif
          : tao::pegtl::if_must<Elif<Implicit>,
                                IfStmtBranch<Implicit, AsyncFlow, LoopFlow,
                                             ScopeFlow, ImportAll>> {};
      struct IfStmtBranchesAction : Stack<asdl::IfBranch> {
        template <typename Outer>
        void success(Outer &&outer) {
          std::vector<asdl::IfBranch> branches;
          branches.reserve(size());
          transform<asdl::IfBranch>(std::back_inserter(branches));
          outer.push(std::move(branches));
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtBranches
          : tao::pegtl::seq<
                IfStmtIf<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                tao::pegtl::star<IfStmtElif<Implicit, AsyncFlow, LoopFlow,
                                            ScopeFlow, ImportAll>>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          IfStmtBranches<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<IfStmtBranches<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                       ImportAll>,
                        IfStmtBranchesAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtElse
          : tao::pegtl::if_must<
                Else<Implicit>,
                Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmt
          : tao::pegtl::seq<
                IfStmtBranches<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                               ImportAll>,
                tao::pegtl::opt<IfStmtElse<Implicit, AsyncFlow, LoopFlow,
                                           ScopeFlow, ImportAll>>> {};
      struct IfStmtAction
          : Stack<std::vector<asdl::IfBranch>, std::vector<asdl::StmtImpl>> {
        template <typename Outer>
        void success(Outer && /*outer*/) {
          // TODO reshape
          if (size() > 1) {
            // outer.push(
            //     asdl::StmtImpl{reshape<asdl::If, std::vector<asdl::IfBranch>,
            //                            std::vector<asdl::StmtImpl>>()});
          } else {
            // outer.push(asdl::StmtImpl{
            //     reshape<asdl::If, std::vector<asdl::IfBranch>>()});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          IfStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<
                IfStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                IfStmtAction> {};
      struct WhileStmtAction
          : Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer && /*outer*/) {
          // TODO reshape
          if (size() > 2) {
            // outer.push(asdl::StmtImpl{reshape<asdl::While, asdl::ExprImpl,
            //                                   std::vector<asdl::StmtImpl>,
            //                                   std::vector<asdl::StmtImpl>>()});
          } else {
            // outer.push(asdl::StmtImpl{reshape<asdl::While, asdl::ExprImpl,
            //                                   std::vector<asdl::StmtImpl>>()});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct WhileStmt
          : tao::pegtl::if_must<
                While<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>,
                Suite<Implicit, AsyncFlow, true, ScopeFlow, ImportAll>,
                tao::pegtl::opt<
                    Else<Implicit>,
                    tao::pegtl::must<Suite<Implicit, AsyncFlow, LoopFlow,
                                           ScopeFlow, ImportAll>>>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          WhileStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<
                WhileStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                WhileStmtAction> {};
      struct ForStmtAction
          : Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer && /*outer*/) {
          // TODO reshape
          if (size() > 3) {
            // outer.push(asdl::StmtImpl{
            //     reshape<asdl::For, asdl::ExprImpl, asdl::ExprImpl,
            //             std::vector<asdl::StmtImpl>,
            //             std::vector<asdl::StmtImpl>>()});
          } else {
            // outer.push(asdl::StmtImpl{
            //     reshape<asdl::For, asdl::ExprImpl, asdl::ExprImpl,
            //             std::vector<asdl::StmtImpl>>()});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct ForStmt
          : tao::pegtl::if_must<
                For<Implicit>, ExprList<Implicit, AsyncFlow, ScopeFlow>,
                In<Implicit>, TestList<Implicit, AsyncFlow, ScopeFlow>,
                Suite<Implicit, AsyncFlow, true, ScopeFlow, ImportAll>,
                tao::pegtl::opt<
                    Else<Implicit>,
                    tao::pegtl::must<Suite<Implicit, AsyncFlow, LoopFlow,
                                           ScopeFlow, ImportAll>>>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          ForStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<
                ForStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                ForStmtAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtTry
          : tao::pegtl::if_must<
                Try<Implicit>,
                Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExceptClause
          : tao::pegtl::seq<
                Except<Implicit>,
                tao::pegtl::opt<
                    Test<Implicit, AsyncFlow, ScopeFlow>,
                    tao::pegtl::opt<As<Implicit>,
                                    tao::pegtl::must<Name<Implicit>>>>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtExcept
          : tao::pegtl::if_must<
                ExceptClause<Implicit, AsyncFlow, ScopeFlow>,
                Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {};
      struct TryStmtExceptAction
          : Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl, asdl::Name> {
        template <typename Outer>
        void success(Outer && /*outer*/) {
          // TODO reshape
          switch (size()) {
            // case 1:
            //   outer.push(
            //       reshape<asdl::ExceptHandler,
            //       std::vector<asdl::StmtImpl>>());
            //   break;
            // case 2:
            //   outer.push(reshape<asdl::ExceptHandler, asdl::ExprImpl,
            //                      std::vector<asdl::StmtImpl>>());
            //   break;
            // case 3:
            //   outer.push(reshape<asdl::ExceptHandler, asdl::ExprImpl,
            //                      asdl::Name, std::vector<asdl::StmtImpl>>());
            //   break;
            default:
              Ensures(false);
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          TryStmtExcept<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<TryStmtExcept<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                      ImportAll>,
                        TryStmtExceptAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtElse
          : tao::pegtl::if_must<
                Else<Implicit>,
                Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtFinally
          : tao::pegtl::if_must<
                Finally<Implicit>,
                Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtExcepts
          : tao::pegtl::plus<TryStmtExcept<Implicit, AsyncFlow, LoopFlow,
                                           ScopeFlow, ImportAll>> {};
      struct TryStmtExceptsAction : Stack<asdl::ExceptHandler> {
        template <typename Outer>
        void success(Outer &&outer) {
          std::vector<asdl::Excepthandler> body;
          body.reserve(size());
          transform<asdl::ExceptHandler>(std::back_inserter(body));
          outer.push(std::move(body));
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          TryStmtExcepts<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<TryStmtExcepts<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                       ImportAll>,
                        TryStmtExceptsAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmt
          : tao::pegtl::seq<
                TryStmtTry<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                tao::pegtl::sor<
                    tao::pegtl::seq<
                        TryStmtExcepts<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                       ImportAll>,
                        tao::pegtl::opt<
                            TryStmtElse<Implicit, AsyncFlow, LoopFlow,
                                        ScopeFlow, ImportAll>>,
                        tao::pegtl::opt<
                            TryStmtFinally<Implicit, AsyncFlow, LoopFlow,
                                           ScopeFlow, ImportAll>>>,
                    TryStmtFinally<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                   ImportAll>>> {};
      struct TryStmtAction : Stack<std::vector<asdl::StmtImpl>,
                                   std::vector<asdl::Excepthandler>> {
        template <typename Outer>
        void success(Outer && /*outer*/) {
          // TODO reshape
          // outer.push(
          //     asdl::StmtImpl{reshape<asdl::Try, std::vector<asdl::StmtImpl>,
          //                            std::vector<asdl::Excepthandler>>()});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          TryStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<
                TryStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                TryStmtAction> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct WithItem
          : tao::pegtl::seq<
                Test<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::opt<As<Implicit>,
                                Expr<Implicit, AsyncFlow, ScopeFlow>>> {};
      struct WithItemAction : Stack<asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          if (size() > 1) {
            // TODO reshape
            // outer.push(
            //     reshape<asdl::Withitem, asdl::ExprImpl, asdl::ExprImpl>());
          } else {
            outer.push(reshape<asdl::Withitem, asdl::ExprImpl>());
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<WithItem<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<WithItem<Implicit, AsyncFlow, ScopeFlow>,
                        WithItemAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct WithStmt
          : tao::pegtl::if_must<
                With<Implicit>,
                tao::pegtl::list_must<WithItem<Implicit, AsyncFlow, ScopeFlow>,
                                      Comma<Implicit>>,
                Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {};
      struct WithStmtAction
          : Stack<std::vector<asdl::StmtImpl>, asdl::Withitem> {
        template <typename Outer>
        void success(Outer &&outer) {
          asdl::With with;
          with.body = pop<std::vector<asdl::StmtImpl>>();
          with.items.reserve(size());
          transform<asdl::Withitem>(std::back_inserter(with.items));
          outer.push(asdl::StmtImpl{std::move(with)});
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          WithStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<
                WithStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                WithStmtAction> {};
      template <bool Implicit>
      struct ClassDef
          : tao::pegtl::if_must<
                Class<Implicit>, Name<Implicit>,
                tao::pegtl::opt<ParenOpt<Implicit, false, false, ArgList>>,
                SuiteWithDoc<Implicit, false, false, false, false>> {};
      struct ClassDefAction
          : Stack<asdl::DocString, std::vector<asdl::StmtImpl>, asdl::ExprImpl,
                  asdl::Keyword, asdl::Name> {
        template <typename Outer>
        void success(Outer &&outer) {
          // TODO reshape
          asdl::ClassDef classDef;
          classDef.body = pop<std::vector<asdl::StmtImpl>>();
          if (top_is<asdl::DocString>()) {
            classDef.doc_string = pop<asdl::DocString>();
          }
          classDef.keywords.reserve(size());
          classDef.bases.reserve(size());
          while (size() > 1) {
            if (top_is<asdl::Keyword>()) {
              classDef.keywords.push_back(pop<asdl::Keyword>());
            } else {
              classDef.bases.push_back(pop<asdl::ExprImpl>());
            }
          }
          classDef.name = pop<asdl::Name>();
          outer.push(asdl::StmtImpl{std::move(classDef)});
        }
      };
      template <bool Implicit>
      struct Control<ClassDef<Implicit>>
          : ChangeState<ClassDef<Implicit>, ClassDefAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow>
      struct FlowStmt
          : tao::pegtl::sor<BreakStmt<Implicit, LoopFlow>,
                            ContinueStmt<Implicit, LoopFlow>,
                            ReturnStmt<Implicit, AsyncFlow, ScopeFlow>,
                            RaiseStmt<Implicit, AsyncFlow, ScopeFlow>,
                            YieldStmt<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool ImportAll>
      struct ImportStmt : tao::pegtl::sor<ImportName<Implicit>,
                                          ImportFrom<Implicit, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SmallStmt
          : tao::pegtl::sor<AssertStmt<Implicit, AsyncFlow, ScopeFlow>,
                            DelStmt<Implicit, AsyncFlow, ScopeFlow>,
                            Pass<Implicit>,
                            FlowStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow>,
                            ImportStmt<Implicit, ImportAll>,
                            GlobalStmt<Implicit>, NonLocalStmt<Implicit>,
                            ExprStmt<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SimpleStmt
          : tao::pegtl::if_must<
                tao::pegtl::list_tail<SmallStmt<Implicit, AsyncFlow, LoopFlow,
                                                ScopeFlow, ImportAll>,
                                      Semi<Implicit>>,
                tao::pegtl::sor<NEWLINE, tao::pegtl::at<tao::pegtl::eolf>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Decorator
          : tao::pegtl::if_must<At<Implicit>,
                                Expr<Implicit, AsyncFlow, ScopeFlow>, NEWLINE> {
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Decorators
          : tao::pegtl::plus<Decorator<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Decorated
          : tao::pegtl::if_must<
                Decorators<Implicit, AsyncFlow, ScopeFlow>,
                tao::pegtl::sor<ClassDef<Implicit>, FuncDef<Implicit, false>,
                                AsyncFuncDef<Implicit>>> {};
      struct DecoratedAction : Stack<asdl::StmtImpl, asdl::ExprImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          auto action = [this](auto &&def) {
            def.decorator_list.reserve(this->size());
            this->template transform<asdl::ExprImpl>(
                std::back_inserter(def.decorator_list));
          };
          auto stmt = pop<asdl::StmtImpl>();
          if (std::holds_alternative<asdl::ClassDef>(*stmt.value)) {
            action(std::get<asdl::ClassDef>(*stmt.value));
          } else if (std::holds_alternative<asdl::FunctionDef>(*stmt.value)) {
            action(std::get<asdl::FunctionDef>(*stmt.value));
          } else {
            action(std::get<asdl::AsyncFunctionDef>(*stmt.value));
          }
          outer.push(std::move(stmt));
        }
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Control<Decorated<Implicit, AsyncFlow, ScopeFlow>>
          : ChangeState<Decorated<Implicit, AsyncFlow, ScopeFlow>,
                        DecoratedAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct AsyncStmt
          : tao::pegtl::if_must<
                Async<Implicit>,
                tao::pegtl::sor<
                    std::conditional_t<
                        AsyncFlow,
                        tao::pegtl::sor<WithStmt<Implicit, true, LoopFlow,
                                                 ScopeFlow, ImportAll>,
                                        ForStmt<Implicit, true, LoopFlow,
                                                ScopeFlow, ImportAll>>,
                        tao::pegtl::failure>,
                    FuncDef<Implicit, true>>> {};
      struct AsyncStmtAction : Stack<asdl::StmtImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          auto stmt = pop<asdl::StmtImpl>();
          if (std::holds_alternative<asdl::FunctionDef>(*stmt.value)) {
            auto functionDef =
                std::get<asdl::FunctionDef>(std::move(*stmt.value));
            outer.push(asdl::StmtImpl{asdl::AsyncFunctionDef{
                std::move(functionDef.name), functionDef.doc_string,
                std::move(functionDef.args), std::move(functionDef.body),
                std::move(functionDef.decorator_list),
                std::move(functionDef.returns)}});
          } else if (std::holds_alternative<asdl::With>(*stmt.value)) {
            auto with = std::get<asdl::With>(std::move(*stmt.value));
            outer.push(asdl::StmtImpl{
                asdl::AsyncWith{std::move(with.items), std::move(with.body)}});
          } else {
            auto asdlFor = std::get<asdl::For>(std::move(*stmt.value));
            outer.push(asdl::StmtImpl{asdl::AsyncFor{
                std::move(asdlFor.target), std::move(asdlFor.iter),
                std::move(asdlFor.body), std::move(asdlFor.orelse)}});
          }
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          AsyncStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<
                AsyncStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                AsyncStmtAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct CompoundStmt
          : tao::pegtl::sor<
                IfStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                WhileStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                ForStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                TryStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                WithStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                FuncDef<Implicit, false>, ClassDef<Implicit>,
                Decorated<Implicit, AsyncFlow, ScopeFlow>,
                AsyncStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                          ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Stmt : tao::pegtl::sor<CompoundStmt<Implicit, AsyncFlow, LoopFlow,
                                                 ScopeFlow, ImportAll>,
                                    SimpleStmt<Implicit, AsyncFlow, LoopFlow,
                                               ScopeFlow, ImportAll>> {};
      struct SuiteAction : Stack<asdl::StmtImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          std::vector<asdl::StmtImpl> body;
          body.reserve(size());
          transform<asdl::StmtImpl>(std::back_inserter(body));
          outer.push(std::move(body));
        }
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SuiteSimpleStmt
          : tao::pegtl::seq<SimpleStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                       ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          SuiteSimpleStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<SuiteSimpleStmt<Implicit, AsyncFlow, LoopFlow,
                                        ScopeFlow, ImportAll>,
                        SuiteAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SuiteSeqStmtPlus
          : tao::pegtl::plus<
                Stmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          SuiteSeqStmtPlus<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<SuiteSeqStmtPlus<Implicit, AsyncFlow, LoopFlow,
                                         ScopeFlow, ImportAll>,
                        SuiteAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SuiteSeqStmtStar
          : tao::pegtl::star<
                Stmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Control<
          SuiteSeqStmtStar<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>>
          : ChangeState<SuiteSeqStmtStar<Implicit, AsyncFlow, LoopFlow,
                                         ScopeFlow, ImportAll>,
                        SuiteAction> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SuiteWithDoc
          : tao::pegtl::if_must<
                Colon<Implicit>,
                tao::pegtl::sor<
                    DocString<Implicit>,
                    SuiteSimpleStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                    ImportAll>,
                    tao::pegtl::if_must<
                        INDENT,
                        tao::pegtl::sor<
                            tao::pegtl::seq<
                                DocString<Implicit>,
                                SuiteSeqStmtStar<Implicit, AsyncFlow, LoopFlow,
                                                 ScopeFlow, ImportAll>>,
                            SuiteSeqStmtPlus<Implicit, AsyncFlow, LoopFlow,
                                             ScopeFlow, ImportAll>>,
                        DEDENT>>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Suite : tao::pegtl::if_must<
                         Colon<Implicit>,
                         tao::pegtl::sor<
                             SuiteSimpleStmt<Implicit, AsyncFlow, LoopFlow,
                                             ScopeFlow, ImportAll>,
                             tao::pegtl::if_must<
                                 INDENT,
                                 SuiteSeqStmtPlus<Implicit, AsyncFlow, LoopFlow,
                                                  ScopeFlow, ImportAll>,
                                 DEDENT>>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
