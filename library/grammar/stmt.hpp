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
      struct TFPDef : Seq<Name<Implicit>,
                          Opt<Colon<Implicit>,
                              Must<Test<Implicit, AsyncFlow, ScopeFlow>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Name> {
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
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TypedargsListTFPDefEqTest
          : Seq<TFPDef<Implicit, AsyncFlow, ScopeFlow>,
                Opt<Eq<Implicit>, Must<Test<Implicit, AsyncFlow, ScopeFlow>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Arg> {
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
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TypedargsListStarTFPDef
          : Seq<StarOp<Implicit>, Opt<TFPDef<Implicit, AsyncFlow, ScopeFlow>>> {
        struct Transform : rules::Stack<asdl::Arg> {
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
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TypedargsListUnpackTfpdef
          : IfMust<Unpack<Implicit>, TFPDef<Implicit, AsyncFlow, ScopeFlow>,
                   Opt<Comma<Implicit>>> {
        struct Transform : rules::Stack<asdl::Arg> {
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
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct TypedArgsList
          : Sor<Seq<TypedargsListTFPDefEqTest<Implicit, AsyncFlow, ScopeFlow>,
                    Star<Comma<Implicit>, TypedargsListTFPDefEqTest<
                                              Implicit, AsyncFlow, ScopeFlow>>,
                    Opt<Comma<Implicit>,
                        Opt<Sor<Seq<TypedargsListStarTFPDef<Implicit, AsyncFlow,
                                                            ScopeFlow>,
                                    Star<Comma<Implicit>,
                                         TypedargsListTFPDefEqTest<
                                             Implicit, AsyncFlow, ScopeFlow>>,
                                    Opt<Comma<Implicit>,
                                        Opt<TypedargsListUnpackTfpdef<
                                            Implicit, AsyncFlow, ScopeFlow>>>>,
                                TypedargsListUnpackTfpdef<Implicit, AsyncFlow,
                                                          ScopeFlow>>>>>,
                Seq<TypedargsListStarTFPDef<Implicit, AsyncFlow, ScopeFlow>,
                    Star<Comma<Implicit>, TypedargsListTFPDefEqTest<
                                              Implicit, AsyncFlow, ScopeFlow>>,
                    Opt<Comma<Implicit>, Opt<TypedargsListUnpackTfpdef<
                                             Implicit, AsyncFlow, ScopeFlow>>>>,
                TypedargsListUnpackTfpdef<Implicit, AsyncFlow, ScopeFlow>> {
        // using Transform = rules::ForwardCapture<asdl::Arguments>;
        struct Transform : rules::Stack<asdl::Arguments> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::Arguments>());
          }
        };
      };
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
          : IfMust<Def<Implicit>, Name<Implicit>,
                   Parameters<Implicit, false, false>,
                   Opt<Arr<Implicit>, Must<Test<Implicit, false, false>>>,
                   SuiteWithDoc<Implicit, AsyncFlow, false, true, false>> {
        struct Transform
            : rules::Stack<asdl::DocString, std::vector<asdl::StmtImpl>,
                           asdl::ExprImpl, asdl::Arguments, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            // TODO(grandquista): reshape
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
      };
      template <bool Implicit>
      struct AsyncFuncDef : Seq<Async<Implicit>, FuncDef<Implicit, true>> {
        struct Transform : rules::Stack<asdl::StmtImpl> {
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
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmtTarget
          : Seq<TestListStarExpr<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
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
      };
      template <bool Implicit>
      struct AugAssign
          : Sor<AugAdd<Implicit>, AugSub<Implicit>, AugMult<Implicit>,
                AugMatMult<Implicit>, AugDiv<Implicit>, AugMod<Implicit>,
                AugBitAnd<Implicit>, AugBitOr<Implicit>, AugBitXor<Implicit>,
                AugLShift<Implicit>, AugRShift<Implicit>, AugPow<Implicit>,
                AugFloorDiv<Implicit>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmtAugAssign
          : IfMust<Seq<ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>,
                       AugAssign<Implicit>>,
                   Sor<YieldExpr<Implicit, AsyncFlow, ScopeFlow>,
                       TestList<Implicit, AsyncFlow, ScopeFlow>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(
                asdl::StmtImpl{reshape<asdl::AugAssign, asdl::ExprImpl,
                                       asdl::Operator, asdl::ExprImpl>()});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmtAssign
          : Seq<ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>, Eq<Implicit>,
                ListMust<Sor<YieldExpr<Implicit, AsyncFlow, ScopeFlow>,
                             ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>>,
                         Eq<Implicit>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Assign assign{{}, pop<asdl::ExprImpl>()};
            assign.targets.reserve(size());
            transform<asdl::ExprImpl>(std::back_inserter(assign.targets));
            outer.push(asdl::StmtImpl{std::move(assign)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AnnAssign
          : IfMust<
                Seq<ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>,
                    Colon<Implicit>>,
                Test<Implicit, AsyncFlow, ScopeFlow>,
                Opt<Eq<Implicit>, Must<Test<Implicit, AsyncFlow, ScopeFlow>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 2) {
              // TODO(grandquista): reshape
              // outer.push(
              //     asdl::StmtImpl{reshape<asdl::AnnAssign, asdl::ExprImpl,
              //                            asdl::ExprImpl, asdl::ExprImpl>()});
            } else {
              outer.push(asdl::StmtImpl{
                  reshape<asdl::AnnAssign, asdl::ExprImpl, asdl::ExprImpl>()});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmtExprStmtTarget
          : Seq<ExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::StmtImpl{asdl::Expr{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExprStmt
          : Sor<AnnAssign<Implicit, AsyncFlow, ScopeFlow>,
                ExprStmtAugAssign<Implicit, AsyncFlow, ScopeFlow>,
                ExprStmtAssign<Implicit, AsyncFlow, ScopeFlow>,
                ExprStmtExprStmtTarget<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct DelStmt
          : IfMust<Del<Implicit>, ExprList<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Delete asdlDelete;
            asdlDelete.targets.reserve(size());
            transform<asdl::ExprImpl>(std::back_inserter(asdlDelete.targets));
            outer.push(asdl::StmtImpl{std::move(asdlDelete)});
          }
        };
      };
      template <bool Implicit, bool LoopFlow>
      struct BreakStmt
          : std::conditional_t<LoopFlow, Break<Implicit>, Failure> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::StmtImpl{asdl::Break{}});
          }
        };
      };
      template <bool Implicit, bool LoopFlow>
      struct ContinueStmt
          : std::conditional_t<LoopFlow, Continue<Implicit>, Failure> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::StmtImpl{asdl::Continue{}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ReturnStmt
          : std::conditional_t<
                ScopeFlow,
                Seq<Return<Implicit>,
                    Opt<TestList<Implicit, AsyncFlow, ScopeFlow>>>,
                Failure> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (has_value()) {
              outer.push(asdl::StmtImpl{asdl::Return{pop<asdl::ExprImpl>()}});
            } else {
              outer.push(asdl::StmtImpl{asdl::Return{}});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct YieldStmt : Seq<YieldExpr<Implicit, AsyncFlow, ScopeFlow>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::StmtImpl{asdl::Expr{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct RaiseStmt : Seq<Raise<Implicit>,
                             Opt<Test<Implicit, AsyncFlow, ScopeFlow>,
                                 Opt<From<Implicit>,
                                     Test<Implicit, AsyncFlow, ScopeFlow>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
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
      };
      template <bool Implicit>
      struct DottedName : List<Name<Implicit>, Period<Implicit>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::vector<asdl::Name> identifiers;
            identifiers.reserve(size());
            transform<asdl::Name>(std::back_inserter(identifiers));
            outer.push(
                std::reduce(identifiers.begin(), identifiers.end(),
                            asdl::Name{}, [](const auto &ida, const auto &idb) {
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
      };
      template <bool Implicit>
      struct DottedAsName
          : Seq<DottedName<Implicit>, Opt<As<Implicit>, Name<Implicit>>> {
        struct Transform : rules::Stack<asdl::Name> {
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
      };
      template <bool Implicit>
      struct DottedAsNames : List<DottedAsName<Implicit>, Comma<Implicit>> {};
      template <bool Implicit>
      struct ImportName : Seq<Import<Implicit>, DottedAsNames<Implicit>> {
        struct Transform : rules::Stack<asdl::Alias> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Import import;
            import.names.reserve(size());
            transform<asdl::Alias>(std::back_inserter(import.names));
            outer.push(asdl::StmtImpl{std::move(import)});
          }
        };
      };
      template <bool Implicit>
      struct ImportFromPeriod : Seq<PeriodImpl<Implicit>> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            std::get<asdl::ImportFrom>(
                *outer.template top<asdl::StmtImpl>().value)
                .module.value.append(1, '.');
          }
        };
      };
      template <bool Implicit>
      struct ImportFromModuleName
          : Seq<NotAt<Import<Implicit>>, DottedName<Implicit>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::get<asdl::ImportFrom>(
                *outer.template top<asdl::StmtImpl>().value)
                .module.value.append(pop<asdl::Name>().value);
          }
        };
      };
      template <bool Implicit>
      struct ImportAsName
          : Seq<Name<Implicit>, Opt<As<Implicit>, Name<Implicit>>> {
        struct Transform : rules::Stack<asdl::Name> {
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
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ImportAsNames : ListTail<ImportAsName<Implicit>, Comma<Implicit>> {
        struct Transform : rules::Stack<asdl::Alias> {
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
      };
      template <bool Implicit, bool ImportAll>
      struct ImportFrom
          : IfMust<From<Implicit>,
                   Sor<Seq<Star<ImportFromPeriod<Implicit>>,
                           ImportFromModuleName<Implicit>>,
                       Plus<PeriodImpl<Implicit>>>,
                   Import<Implicit>,
                   Sor<std::conditional_t<ImportAll, StarOp<Implicit>, Failure>,
                       Paren<Implicit, false, false, ImportAsNames>,
                       ImportAsNames<Implicit, false, false>>> {
        struct Transform : rules::Stack<asdl::StmtImpl> {
          Transform() { push(asdl::StmtImpl{asdl::ImportFrom{}}); }
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::StmtImpl>());
          }
        };
      };
      template <bool Implicit>
      struct GlobalStmt : IfMust<Global<Implicit>,
                                 ListMust<Name<Implicit>, Comma<Implicit>>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Global global;
            global.names.reserve(size());
            transform<asdl::Name>(std::back_inserter(global.names));
            outer.push(asdl::StmtImpl{std::move(global)});
          }
        };
      };
      template <bool Implicit>
      struct NonLocalStmt : IfMust<Nonlocal<Implicit>,
                                   ListMust<Name<Implicit>, Comma<Implicit>>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Nonlocal nonlocal;
            nonlocal.names.reserve(size());
            transform<asdl::Name>(std::back_inserter(nonlocal.names));
            outer.push(asdl::StmtImpl{std::move(nonlocal)});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct AssertStmt
          : IfMust<Assert<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>,
                   Opt<Comma<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              // TODO(grandquista): reshape
              // outer.push(asdl::StmtImpl{
              //     reshape<asdl::Assert, asdl::ExprImpl, asdl::ExprImpl>()});
            } else {
              outer.push(
                  asdl::StmtImpl{reshape<asdl::Assert, asdl::ExprImpl>()});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtBranch
          : Seq<Test<Implicit, AsyncFlow, ScopeFlow>,
                Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer && /*outer*/) {
            // TODO(grandquista): reshape
            // outer.push(reshape<asdl::IfBranch, asdl::ExprImpl,
            //                    std::vector<asdl::StmtImpl>>());
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtIf
          : IfMust<If<Implicit>, IfStmtBranch<Implicit, AsyncFlow, LoopFlow,
                                              ScopeFlow, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtElif
          : IfMust<Elif<Implicit>, IfStmtBranch<Implicit, AsyncFlow, LoopFlow,
                                                ScopeFlow, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtBranches
          : Seq<IfStmtIf<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                Star<IfStmtElif<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                ImportAll>>> {
        struct Transform : rules::Stack<asdl::IfBranch> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::vector<asdl::IfBranch> branches;
            branches.reserve(size());
            transform<asdl::IfBranch>(std::back_inserter(branches));
            outer.push(std::move(branches));
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmtElse
          : IfMust<Else<Implicit>,
                   Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct IfStmt : Seq<IfStmtBranches<Implicit, AsyncFlow, LoopFlow,
                                         ScopeFlow, ImportAll>,
                          Opt<IfStmtElse<Implicit, AsyncFlow, LoopFlow,
                                         ScopeFlow, ImportAll>>> {
        struct Transform : rules::Stack<std::vector<asdl::IfBranch>,
                                        std::vector<asdl::StmtImpl>> {
          template <typename Outer>
          void success(Outer && /*outer*/) {
            // TODO(grandquista): reshape
            if (size() > 1) {
              // outer.push(
              //     asdl::StmtImpl{reshape<asdl::If,
              //     std::vector<asdl::IfBranch>,
              //                            std::vector<asdl::StmtImpl>>()});
            } else {
              // outer.push(asdl::StmtImpl{
              //     reshape<asdl::If, std::vector<asdl::IfBranch>>()});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct WhileStmt
          : IfMust<While<Implicit>, Test<Implicit, AsyncFlow, ScopeFlow>,
                   Suite<Implicit, AsyncFlow, true, ScopeFlow, ImportAll>,
                   Opt<Else<Implicit>, Must<Suite<Implicit, AsyncFlow, LoopFlow,
                                                  ScopeFlow, ImportAll>>>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer && /*outer*/) {
            // TODO(grandquista): reshape
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
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct ForStmt
          : IfMust<For<Implicit>, ExprList<Implicit, AsyncFlow, ScopeFlow>,
                   In<Implicit>, TestList<Implicit, AsyncFlow, ScopeFlow>,
                   Suite<Implicit, AsyncFlow, true, ScopeFlow, ImportAll>,
                   Opt<Else<Implicit>, Must<Suite<Implicit, AsyncFlow, LoopFlow,
                                                  ScopeFlow, ImportAll>>>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer && /*outer*/) {
            // TODO(grandquista): reshape
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
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtTry
          : IfMust<Try<Implicit>,
                   Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct ExceptClause : Seq<Except<Implicit>,
                                Opt<Test<Implicit, AsyncFlow, ScopeFlow>,
                                    Opt<As<Implicit>, Must<Name<Implicit>>>>> {
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtExcept
          : IfMust<ExceptClause<Implicit, AsyncFlow, ScopeFlow>,
                   Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {
        struct Transform : rules::Stack<std::vector<asdl::StmtImpl>,
                                        asdl::ExprImpl, asdl::Name> {
          template <typename Outer>
          void success(Outer && /*outer*/) {
            // TODO(grandquista): reshape
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
              //                      asdl::Name,
              //                      std::vector<asdl::StmtImpl>>());
              //   break;
              default:
                Ensures(false);
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtElse
          : IfMust<Else<Implicit>,
                   Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtFinally
          : IfMust<Finally<Implicit>,
                   Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmtExcepts : Plus<TryStmtExcept<Implicit, AsyncFlow, LoopFlow,
                                                 ScopeFlow, ImportAll>> {
        struct Transform : rules::Stack<asdl::ExceptHandler> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::vector<asdl::Excepthandler> body;
            body.reserve(size());
            transform<asdl::ExceptHandler>(std::back_inserter(body));
            outer.push(std::move(body));
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct TryStmt
          : Seq<TryStmtTry<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
                Sor<Seq<TryStmtExcepts<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                       ImportAll>,
                        Opt<TryStmtElse<Implicit, AsyncFlow, LoopFlow,
                                        ScopeFlow, ImportAll>>,
                        Opt<TryStmtFinally<Implicit, AsyncFlow, LoopFlow,
                                           ScopeFlow, ImportAll>>>,
                    TryStmtFinally<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                   ImportAll>>> {
        struct Transform : rules::Stack<std::vector<asdl::StmtImpl>,
                                        std::vector<asdl::Excepthandler>> {
          template <typename Outer>
          void success(Outer && /*outer*/) {
            // TODO(grandquista): reshape
            // outer.push(
            //     asdl::StmtImpl{reshape<asdl::Try,
            //     std::vector<asdl::StmtImpl>,
            //                            std::vector<asdl::Excepthandler>>()});
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct WithItem
          : Seq<Test<Implicit, AsyncFlow, ScopeFlow>,
                Opt<As<Implicit>, Expr<Implicit, AsyncFlow, ScopeFlow>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              // TODO(grandquista): reshape
              // outer.push(
              //     reshape<asdl::Withitem, asdl::ExprImpl, asdl::ExprImpl>());
            } else {
              outer.push(reshape<asdl::Withitem, asdl::ExprImpl>());
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct WithStmt
          : IfMust<With<Implicit>,
                   ListMust<WithItem<Implicit, AsyncFlow, ScopeFlow>,
                            Comma<Implicit>>,
                   Suite<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::Withitem> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::With with;
            with.body = pop<std::vector<asdl::StmtImpl>>();
            with.items.reserve(size());
            transform<asdl::Withitem>(std::back_inserter(with.items));
            outer.push(asdl::StmtImpl{std::move(with)});
          }
        };
      };
      template <bool Implicit>
      struct ClassDef
          : IfMust<Class<Implicit>, Name<Implicit>,
                   Opt<ParenOpt<Implicit, false, false, ArgList>>,
                   SuiteWithDoc<Implicit, false, false, false, false>> {
        struct Transform
            : rules::Stack<asdl::DocString, std::vector<asdl::StmtImpl>,
                           asdl::ExprImpl, asdl::Keyword, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            // TODO(grandquista): reshape
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
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow>
      struct FlowStmt
          : Sor<BreakStmt<Implicit, LoopFlow>, ContinueStmt<Implicit, LoopFlow>,
                ReturnStmt<Implicit, AsyncFlow, ScopeFlow>,
                RaiseStmt<Implicit, AsyncFlow, ScopeFlow>,
                YieldStmt<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool ImportAll>
      struct ImportStmt
          : Sor<ImportName<Implicit>, ImportFrom<Implicit, ImportAll>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SmallStmt
          : Sor<AssertStmt<Implicit, AsyncFlow, ScopeFlow>,
                DelStmt<Implicit, AsyncFlow, ScopeFlow>, Pass<Implicit>,
                FlowStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow>,
                ImportStmt<Implicit, ImportAll>, GlobalStmt<Implicit>,
                NonLocalStmt<Implicit>,
                ExprStmt<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SimpleStmt
          : IfMust<ListTail<SmallStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                      ImportAll>,
                            Semi<Implicit>>,
                   Sor<NEWLINE, At<Eolf>>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Decorator : IfMust<AtOp<Implicit>,
                                Expr<Implicit, AsyncFlow, ScopeFlow>, NEWLINE> {
      };
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Decorators : Plus<Decorator<Implicit, AsyncFlow, ScopeFlow>> {};
      template <bool Implicit, bool AsyncFlow, bool ScopeFlow>
      struct Decorated
          : IfMust<Decorators<Implicit, AsyncFlow, ScopeFlow>,
                   Sor<ClassDef<Implicit>, FuncDef<Implicit, false>,
                       AsyncFuncDef<Implicit>>> {
        struct Transform : rules::Stack<asdl::StmtImpl, asdl::ExprImpl> {
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
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct AsyncStmt
          : IfMust<Async<Implicit>,
                   Sor<std::conditional_t<AsyncFlow,
                                          Sor<WithStmt<Implicit, true, LoopFlow,
                                                       ScopeFlow, ImportAll>,
                                              ForStmt<Implicit, true, LoopFlow,
                                                      ScopeFlow, ImportAll>>,
                                          Failure>,
                       FuncDef<Implicit, true>>> {
        struct Transform : rules::Stack<asdl::StmtImpl> {
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
              outer.push(asdl::StmtImpl{asdl::AsyncWith{std::move(with.items),
                                                        std::move(with.body)}});
            } else {
              auto asdlFor = std::get<asdl::For>(std::move(*stmt.value));
              outer.push(asdl::StmtImpl{asdl::AsyncFor{
                  std::move(asdlFor.target), std::move(asdlFor.iter),
                  std::move(asdlFor.body), std::move(asdlFor.orelse)}});
            }
          }
        };
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct CompoundStmt
          : Sor<IfStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>,
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
      struct Stmt : Sor<CompoundStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                     ImportAll>,
                        SimpleStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                   ImportAll>> {};
      struct SuiteAction : rules::Stack<asdl::StmtImpl> {
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
      struct SuiteSimpleStmt : Seq<SimpleStmt<Implicit, AsyncFlow, LoopFlow,
                                              ScopeFlow, ImportAll>> {
        using Transform = SuiteAction;
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SuiteSeqStmtPlus
          : Plus<Stmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {
        using Transform = SuiteAction;
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SuiteSeqStmtStar
          : Star<Stmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow, ImportAll>> {
        using Transform = SuiteAction;
      };
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct SuiteWithDoc
          : IfMust<
                Colon<Implicit>,
                Sor<DocString<Implicit>,
                    SuiteSimpleStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                    ImportAll>,
                    IfMust<
                        INDENT,
                        Sor<Seq<DocString<Implicit>,
                                SuiteSeqStmtStar<Implicit, AsyncFlow, LoopFlow,
                                                 ScopeFlow, ImportAll>>,
                            SuiteSeqStmtPlus<Implicit, AsyncFlow, LoopFlow,
                                             ScopeFlow, ImportAll>>,
                        DEDENT>>> {};
      template <bool Implicit, bool AsyncFlow, bool LoopFlow, bool ScopeFlow,
                bool ImportAll>
      struct Suite
          : IfMust<Colon<Implicit>,
                   Sor<SuiteSimpleStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                       ImportAll>,
                       IfMust<INDENT,
                              SuiteSeqStmtPlus<Implicit, AsyncFlow, LoopFlow,
                                               ScopeFlow, ImportAll>,
                              DEDENT>>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
