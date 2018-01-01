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

#include "asdl/asdl.hpp"
#include "grammar/expr.hpp"
#include "grammar/keys.hpp"
#include "grammar/oper.hpp"
#include "grammar/rules.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <typename Option>
      struct TFPDef
          : Seq<Name<Option>, Opt<Colon<Option>, Must<Test<Option>>>> {
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
      template <typename Option>
      struct TypedargsListTFPDefEqTest
          : Seq<TFPDef<Option>, Opt<Eq<Option>, Must<Test<Option>>>> {
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
      template <typename Option>
      struct TypedargsListStarTFPDef
          : Seq<StarOp<Option>, Opt<TFPDef<Option>>> {
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
      template <typename Option>
      struct TypedargsListUnpackTfpdef
          : IfMust<Unpack<Option>, TFPDef<Option>, Opt<Comma<Option>>> {
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
      template <typename Option>
      struct TypedArgsList
          : Sor<Seq<TypedargsListTFPDefEqTest<Option>,
                    Star<Comma<Option>, TypedargsListTFPDefEqTest<Option>>,
                    Opt<Comma<Option>,
                        Opt<Sor<
                            Seq<TypedargsListStarTFPDef<Option>,
                                Star<Comma<Option>,
                                     TypedargsListTFPDefEqTest<Option>>,
                                Opt<Comma<Option>,
                                    Opt<TypedargsListUnpackTfpdef<Option>>>>,
                            TypedargsListUnpackTfpdef<Option>>>>>,
                Seq<TypedargsListStarTFPDef<Option>,
                    Star<Comma<Option>, TypedargsListTFPDefEqTest<Option>>,
                    Opt<Comma<Option>, Opt<TypedargsListUnpackTfpdef<Option>>>>,
                TypedargsListUnpackTfpdef<Option>> {
        // using Transform = rules::ForwardCapture<asdl::Arguments>;
        struct Transform : rules::Stack<asdl::Arguments> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::Arguments>());
          }
        };
      };
      template <typename Option>
      struct Parameters : ParenOpt<Option, TypedArgsList> {};
      template <typename Option>
      struct SuiteWithDoc;
      template <typename Option>
      struct Suite;
      template <typename Option>
      struct FuncDef
          : IfMust<
                Def<Option>, Name<Option>,
                Parameters<typename Option::template UnSet<Option::AsyncFlow,
                                                           Option::ScopeFlow>>,
                Opt<Arr<Option>, Must<Test<typename Option::template UnSet<
                                     Option::AsyncFlow, Option::ScopeFlow>>>>,
                SuiteWithDoc<typename Option::template UnSet<
                    Option::LoopFlow,
                    Option::ImportAll>::template Set<Option::ScopeFlow>>> {
        struct Transform
            : rules::Stack<asdl::DocString, std::vector<asdl::StmtImpl>,
                           asdl::ExprImpl, asdl::Arguments, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            // TODO(asakatida): reshape
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
      template <typename Option>
      struct AsyncFuncDef
          : Seq<Async<Option>,
                FuncDef<typename Option::template Set<Option::AsyncFlow>>> {
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
      template <typename Option>
      struct ExprStmtTarget : Seq<TestListStarExpr<Option>> {
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
      template <typename Option>
      struct AugAssign
          : Sor<AugAdd<Option>, AugSub<Option>, AugMult<Option>,
                AugMatMult<Option>, AugDiv<Option>, AugMod<Option>,
                AugBitAnd<Option>, AugBitOr<Option>, AugBitXor<Option>,
                AugLShift<Option>, AugRShift<Option>, AugPow<Option>,
                AugFloorDiv<Option>> {};
      template <typename Option>
      struct ExprStmtAugAssign
          : IfMust<Seq<ExprStmtTarget<Option>, AugAssign<Option>>,
                   Sor<YieldExpr<Option>, TestList<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl, asdl::Operator> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(
                asdl::StmtImpl{reshape<asdl::AugAssign, asdl::ExprImpl,
                                       asdl::Operator, asdl::ExprImpl>()});
          }
        };
      };
      template <typename Option>
      struct ExprStmtAssign
          : Seq<ExprStmtTarget<Option>, Eq<Option>,
                ListMust<Sor<YieldExpr<Option>, ExprStmtTarget<Option>>,
                         Eq<Option>>> {
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
      template <typename Option>
      struct AnnAssign
          : IfMust<Seq<ExprStmtTarget<Option>, Colon<Option>>, Test<Option>,
                   Opt<Eq<Option>, Must<Test<Option>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 2) {
              // TODO(asakatida): reshape
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
      template <typename Option>
      struct ExprStmtExprStmtTarget : Seq<ExprStmtTarget<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::StmtImpl{asdl::Expr{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct ExprStmt
          : Sor<AnnAssign<Option>, ExprStmtAugAssign<Option>,
                ExprStmtAssign<Option>, ExprStmtExprStmtTarget<Option>> {};
      template <typename Option>
      struct DelStmt : IfMust<Del<Option>, ExprList<Option>> {
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
      template <typename Option>
      struct BreakStmt
          : std::conditional_t<Option::template Get<Option::LoopFlow>,
                               Break<Option>, Failure> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::StmtImpl{asdl::Break{}});
          }
        };
      };
      template <typename Option>
      struct ContinueStmt
          : std::conditional_t<Option::template Get<Option::LoopFlow>,
                               Continue<Option>, Failure> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::StmtImpl{asdl::Continue{}});
          }
        };
      };
      template <typename Option>
      struct ReturnStmt
          : std::conditional_t<Option::template Get<Option::ScopeFlow>,
                               Seq<Return<Option>, Opt<TestList<Option>>>,
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
      template <typename Option>
      struct YieldStmt : Seq<YieldExpr<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::StmtImpl{asdl::Expr{pop<asdl::ExprImpl>()}});
          }
        };
      };
      template <typename Option>
      struct RaiseStmt
          : Seq<Raise<Option>,
                Opt<Test<Option>, Opt<From<Option>, Test<Option>>>> {
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
      template <typename Option>
      struct DottedName : List<Name<Option>, Period<Option>> {
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
      template <typename Option>
      struct DottedAsName
          : Seq<DottedName<Option>, Opt<As<Option>, Name<Option>>> {
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
      template <typename Option>
      struct DottedAsNames : List<DottedAsName<Option>, Comma<Option>> {};
      template <typename Option>
      struct ImportName : Seq<Import<Option>, DottedAsNames<Option>> {
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
      template <typename Option>
      struct ImportFromPeriod : Seq<op::PeriodImpl<Option>> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            std::get<asdl::ImportFrom>(
                *outer.template top<asdl::StmtImpl>().value)
                .module.value.append(1, '.');
          }
        };
      };
      template <typename Option>
      struct ImportFromModuleName
          : Seq<NotAt<Import<Option>>, DottedName<Option>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::get<asdl::ImportFrom>(
                *outer.template top<asdl::StmtImpl>().value)
                .module.value.append(pop<asdl::Name>().value);
          }
        };
      };
      template <typename Option>
      struct ImportAsName : Seq<Name<Option>, Opt<As<Option>, Name<Option>>> {
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
      template <typename Option>
      struct ImportAsNames : ListTail<ImportAsName<Option>, Comma<Option>> {
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
      template <typename Option>
      struct ImportFrom
          : IfMust<
                From<Option>,
                Sor<Seq<Star<ImportFromPeriod<Option>>,
                        ImportFromModuleName<Option>>,
                    Plus<op::PeriodImpl<Option>>>,
                Import<Option>,
                Sor<std::conditional_t<Option::template Get<Option::ImportAll>,
                                       StarOp<Option>, Failure>,
                    Paren<typename Option::template UnSet<Option::AsyncFlow,
                                                          Option::ScopeFlow>,
                          ImportAsNames>,
                    ImportAsNames<typename Option::template UnSet<
                        Option::AsyncFlow, Option::ScopeFlow>>>> {
        struct Transform : rules::Stack<asdl::StmtImpl> {
          Transform() { push(asdl::StmtImpl{asdl::ImportFrom{}}); }
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(pop<asdl::StmtImpl>());
          }
        };
      };
      template <typename Option>
      struct GlobalStmt
          : IfMust<Global<Option>, ListMust<Name<Option>, Comma<Option>>> {
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
      template <typename Option>
      struct NonLocalStmt
          : IfMust<Nonlocal<Option>, ListMust<Name<Option>, Comma<Option>>> {
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
      template <typename Option>
      struct AssertStmt : IfMust<Assert<Option>, Test<Option>,
                                 Opt<Comma<Option>, Test<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              // TODO(asakatida): reshape
              // outer.push(asdl::StmtImpl{
              //     reshape<asdl::Assert, asdl::ExprImpl, asdl::ExprImpl>()});
            } else {
              outer.push(
                  asdl::StmtImpl{reshape<asdl::Assert, asdl::ExprImpl>()});
            }
          }
        };
      };
      template <typename Option>
      struct IfStmtBranch : Seq<Test<Option>, Suite<Option>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            (void)outer;
            // TODO(asakatida): reshape
            // outer.push(reshape<asdl::IfBranch, asdl::ExprImpl,
            //                    std::vector<asdl::StmtImpl>>());
          }
        };
      };
      template <typename Option>
      struct IfStmtIf : IfMust<If<Option>, IfStmtBranch<Option>> {};
      template <typename Option>
      struct IfStmtElif : IfMust<Elif<Option>, IfStmtBranch<Option>> {};
      template <typename Option>
      struct IfStmtBranches : Seq<IfStmtIf<Option>, Star<IfStmtElif<Option>>> {
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
      template <typename Option>
      struct IfStmtElse : IfMust<Else<Option>, Suite<Option>> {};
      template <typename Option>
      struct IfStmt : Seq<IfStmtBranches<Option>, Opt<IfStmtElse<Option>>> {
        struct Transform : rules::Stack<std::vector<asdl::IfBranch>,
                                        std::vector<asdl::StmtImpl>> {
          template <typename Outer>
          void success(Outer &&outer) {
            (void)outer;
            // TODO(asakatida): reshape
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
      template <typename Option>
      struct WhileStmt
          : IfMust<While<Option>, Test<Option>,
                   Suite<typename Option::template Set<Option::LoopFlow>>,
                   Opt<Else<Option>, Must<Suite<Option>>>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            (void)outer;
            // TODO(asakatida): reshape
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
      template <typename Option>
      struct ForStmt
          : IfMust<For<Option>, ExprList<Option>, In<Option>, TestList<Option>,
                   Suite<typename Option::template Set<Option::LoopFlow>>,
                   Opt<Else<Option>, Must<Suite<Option>>>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            (void)outer;
            // TODO(asakatida): reshape
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
      template <typename Option>
      struct TryStmtTry : IfMust<Try<Option>, Suite<Option>> {};
      template <typename Option>
      struct ExceptClause
          : Seq<Except<Option>,
                Opt<Test<Option>, Opt<As<Option>, Must<Name<Option>>>>> {};
      template <typename Option>
      struct TryStmtExcept : IfMust<ExceptClause<Option>, Suite<Option>> {
        struct Transform : rules::Stack<std::vector<asdl::StmtImpl>,
                                        asdl::ExprImpl, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            (void)outer;
            // TODO(asakatida): reshape
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
      template <typename Option>
      struct TryStmtElse : IfMust<Else<Option>, Suite<Option>> {};
      template <typename Option>
      struct TryStmtFinally : IfMust<Finally<Option>, Suite<Option>> {};
      template <typename Option>
      struct TryStmtExcepts : Plus<TryStmtExcept<Option>> {
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
      template <typename Option>
      struct TryStmt
          : Seq<TryStmtTry<Option>,
                Sor<Seq<TryStmtExcepts<Option>, Opt<TryStmtElse<Option>>,
                        Opt<TryStmtFinally<Option>>>,
                    TryStmtFinally<Option>>> {
        struct Transform : rules::Stack<std::vector<asdl::StmtImpl>,
                                        std::vector<asdl::Excepthandler>> {
          template <typename Outer>
          void success(Outer &&outer) {
            (void)outer;
            // TODO(asakatida): reshape
            // outer.push(
            //     asdl::StmtImpl{reshape<asdl::Try,
            //     std::vector<asdl::StmtImpl>,
            //                            std::vector<asdl::Excepthandler>>()});
          }
        };
      };
      template <typename Option>
      struct WithItem : Seq<Test<Option>, Opt<As<Option>, Expr<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              // TODO(asakatida): reshape
              // outer.push(
              //     reshape<asdl::Withitem, asdl::ExprImpl, asdl::ExprImpl>());
            } else {
              outer.push(reshape<asdl::Withitem, asdl::ExprImpl>());
            }
          }
        };
      };
      template <typename Option>
      struct WithStmt
          : IfMust<With<Option>, ListMust<WithItem<Option>, Comma<Option>>,
                   Suite<Option>> {
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
      template <typename Option>
      struct ClassDef
          : IfMust<Class<Option>, Name<Option>,
                   Opt<ParenOpt<typename Option::template UnSet<
                                    Option::AsyncFlow, Option::ScopeFlow>,
                                ArgList>>,
                   SuiteWithDoc<typename Option::template UnSet<
                       Option::AsyncFlow, Option::ImportAll, Option::LoopFlow,
                       Option::ScopeFlow>>> {
        struct Transform
            : rules::Stack<asdl::DocString, std::vector<asdl::StmtImpl>,
                           asdl::ExprImpl, asdl::Keyword, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            // TODO(asakatida): reshape
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
      template <typename Option>
      struct FlowStmt
          : Sor<BreakStmt<Option>, ContinueStmt<Option>, ReturnStmt<Option>,
                RaiseStmt<Option>, YieldStmt<Option>> {};
      template <typename Option>
      struct ImportStmt : Sor<ImportName<Option>, ImportFrom<Option>> {};
      template <typename Option>
      struct SmallStmt
          : Sor<AssertStmt<Option>, DelStmt<Option>, Pass<Option>,
                FlowStmt<Option>, ImportStmt<Option>, GlobalStmt<Option>,
                NonLocalStmt<Option>, ExprStmt<Option>> {};
      template <typename Option>
      struct SimpleStmt : IfMust<ListTail<SmallStmt<Option>, Semi<Option>>,
                                 Sor<NEWLINE<Option>, At<Eolf>>> {};
      template <typename Option>
      struct Decorator : IfMust<AtOp<Option>, Expr<Option>, NEWLINE<Option>> {};
      template <typename Option>
      struct Decorators : Plus<Decorator<Option>> {};
      template <typename Option>
      struct Decorated
          : IfMust<
                Decorators<Option>,
                Sor<ClassDef<Option>,
                    FuncDef<typename Option::template UnSet<Option::AsyncFlow>>,
                    AsyncFuncDef<Option>>> {
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
      template <typename Option>
      struct AsyncStmt
          : IfMust<Async<Option>,
                   Sor<std::conditional_t<
                           Option::template Get<Option::AsyncFlow>,
                           Sor<WithStmt<Option>, ForStmt<Option>>, Failure>,
                       FuncDef<
                           typename Option::template Set<Option::AsyncFlow>>>> {
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
      template <typename Option>
      struct CompoundStmt
          : Sor<IfStmt<Option>, WhileStmt<Option>, ForStmt<Option>,
                TryStmt<Option>, WithStmt<Option>,
                FuncDef<typename Option::template UnSet<Option::AsyncFlow>>,
                ClassDef<Option>, Decorated<Option>, AsyncStmt<Option>> {};
      template <typename Option>
      struct Stmt : Sor<CompoundStmt<Option>, SimpleStmt<Option>> {};
      struct SuiteAction : rules::Stack<asdl::StmtImpl> {
        template <typename Outer>
        void success(Outer &&outer) {
          std::vector<asdl::StmtImpl> body;
          body.reserve(size());
          transform<asdl::StmtImpl>(std::back_inserter(body));
          outer.push(std::move(body));
        }
      };
      template <typename Option>
      struct SuiteSimpleStmt : Seq<SimpleStmt<Option>> {
        using Transform = SuiteAction;
      };
      template <typename Option>
      struct SuiteSeqStmtPlus : Plus<Stmt<Option>> {
        using Transform = SuiteAction;
      };
      template <typename Option>
      struct SuiteSeqStmtStar : Star<Stmt<Option>> {
        using Transform = SuiteAction;
      };
      template <typename Option>
      struct SuiteWithDoc
          : IfMust<
                Colon<Option>,
                Sor<DocString<Option>, SuiteSimpleStmt<Option>,
                    IfMust<INDENT<Option>,
                           Sor<Seq<DocString<Option>, SuiteSeqStmtStar<Option>>,
                               SuiteSeqStmtPlus<Option>>,
                           DEDENT<Option>>>> {};
      template <typename Option>
      struct Suite : IfMust<Colon<Option>,
                            Sor<SuiteSimpleStmt<Option>,
                                IfMust<INDENT<Option>, SuiteSeqStmtPlus<Option>,
                                       DEDENT<Option>>>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
