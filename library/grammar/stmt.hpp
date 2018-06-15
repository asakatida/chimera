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

#include "asdl/asdl.hpp"
#include "grammar/expr.hpp"
#include "grammar/flags.hpp"
#include "grammar/keys.hpp"
#include "grammar/oper.hpp"
#include "grammar/rules.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <flags::Flag Option>
      struct SuiteSimpleStmt;
      template <flags::Flag Option>
      struct SuiteSeqStmtPlus;
      template <flags::Flag Option>
      struct SuiteSeqStmtStar;
      template <flags::Flag Option>
      using SuiteWithDoc = if_must<
          Colon<Option>,
          sor<DocString<Option>, SuiteSimpleStmt<Option>,
              if_must<INDENT,
                      sor<seq<DocString<Option>, SuiteSeqStmtStar<Option>>,
                          SuiteSeqStmtPlus<Option>>,
                      DEDENT>>>;
      template <flags::Flag Option>
      using Suite =
          if_must<Colon<Option>,
                  sor<SuiteSimpleStmt<Option>,
                      if_must<INDENT, SuiteSeqStmtPlus<Option>, DEDENT>>>;
      template <flags::Flag Option>
      struct TFPDef
          : seq<Name<Option>, opt<Colon<Option>, must<Test<Option>>>> {
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
      template <flags::Flag Option>
      struct TypedargsListTFPDefEqTest
          : seq<TFPDef<Option>, opt<Eq<Option>, must<Test<Option>>>> {
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
      template <flags::Flag Option>
      struct TypedargsListStarTFPDef
          : seq<StarOp<Option>, opt<TFPDef<Option>>> {
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
      template <flags::Flag Option>
      struct TypedargsListUnpackTfpdef
          : if_must<Unpack<Option>, TFPDef<Option>, opt<Comma<Option>>> {
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
      template <flags::Flag Option>
      struct TypedArgsList
          : sor<seq<TypedargsListTFPDefEqTest<Option>,
                    star<Comma<Option>, TypedargsListTFPDefEqTest<Option>>,
                    opt<Comma<Option>,
                        opt<sor<
                            seq<TypedargsListStarTFPDef<Option>,
                                star<Comma<Option>,
                                     TypedargsListTFPDefEqTest<Option>>,
                                opt<Comma<Option>,
                                    opt<TypedargsListUnpackTfpdef<Option>>>>,
                            TypedargsListUnpackTfpdef<Option>>>>>,
                seq<TypedargsListStarTFPDef<Option>,
                    star<Comma<Option>, TypedargsListTFPDefEqTest<Option>>,
                    opt<Comma<Option>, opt<TypedargsListUnpackTfpdef<Option>>>>,
                TypedargsListUnpackTfpdef<Option>> {};
      template <flags::Flag Option>
      using Parameters =
          if_must<LPar<flags::set<Option, flags::IMPLICIT>>,
                  opt<TypedArgsList<flags::set<Option, flags::IMPLICIT>>>,
                  RPar<Option>>;
      template <flags::Flag Option>
      struct FuncDef
          : if_must<
                Def<Option>, Name<Option>,
                Parameters<
                    flags::unSet<Option, flags::ASYNC_FLOW, flags::SCOPE_FLOW>>,
                opt<Arr<Option>,
                    must<Test<flags::unSet<Option, flags::ASYNC_FLOW,
                                           flags::SCOPE_FLOW>>>>,
                SuiteWithDoc<flags::set<
                    flags::unSet<Option, flags::LOOP_FLOW, flags::IMPORT_ALL>,
                    flags::SCOPE_FLOW>>> {
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
            outer.push(std::move(functionDef));
          }
        };
      };
      template <flags::Flag Option>
      struct AsyncFuncDef
          : seq<Async<Option>, FuncDef<flags::set<Option, flags::ASYNC_FLOW>>> {
        struct Transform : rules::Stack<asdl::FunctionDef> {
          template <typename Outer>
          void success(Outer &&outer) {
            auto functionDef = pop<asdl::FunctionDef>();
            outer.push(asdl::AsyncFunctionDef{
                std::move(functionDef.name), std::move(functionDef.doc_string),
                std::move(functionDef.args), std::move(functionDef.body),
                std::move(functionDef.decorator_list),
                std::move(functionDef.returns)});
          }
        };
      };
      template <flags::Flag Option>
      struct ExprStmtTarget : seq<TestListStarExpr<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (auto s = size(); s > 1) {
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
      using AugAssign =
          sor<AugAdd<Option>, AugSub<Option>, AugMult<Option>,
              AugMatMult<Option>, AugDiv<Option>, AugMod<Option>,
              AugBitAnd<Option>, AugBitOr<Option>, AugBitXor<Option>,
              AugLShift<Option>, AugRShift<Option>, AugPow<Option>,
              AugFloorDiv<Option>>;
      template <flags::Flag Option>
      struct ExprStmtAugAssign
          : if_must<seq<ExprStmtTarget<Option>, AugAssign<Option>>,
                    sor<YieldExpr<Option>, TestList<Option>>> {
        using Transform = rules::ReshapeCapture<asdl::AugAssign, asdl::ExprImpl,
                                                asdl::Operator, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct ExprStmtAssign
          : seq<ExprStmtTarget<Option>, Eq<Option>,
                list_must<sor<YieldExpr<Option>, ExprStmtTarget<Option>>,
                          Eq<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Assign assign{{}, pop<asdl::ExprImpl>()};
            assign.targets.reserve(size());
            transform<asdl::ExprImpl>(std::back_inserter(assign.targets));
            outer.push(std::move(assign));
          }
        };
      };
      template <flags::Flag Option>
      struct AnnAssign
          : if_must<seq<ExprStmtTarget<Option>, Colon<Option>>, Test<Option>,
                    opt<Eq<Option>, must<Test<Option>>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 2) {
              outer.push(reshape<asdl::AnnAssign, asdl::ExprImpl,
                                 asdl::ExprImpl, asdl::ExprImpl>());
            } else {
              outer.push(
                  reshape<asdl::AnnAssign, asdl::ExprImpl, asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct ExprStmtExprStmtTarget : seq<ExprStmtTarget<Option>> {
        using Transform = rules::ReshapeCapture<asdl::Expr, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      using ExprStmt =
          sor<AnnAssign<Option>, ExprStmtAugAssign<Option>,
              ExprStmtAssign<Option>, ExprStmtExprStmtTarget<Option>>;
      template <flags::Flag Option>
      struct DelStmt : if_must<Del<Option>, ExprList<Option>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Delete asdlDelete;
            asdlDelete.targets.reserve(size());
            transform<asdl::ExprImpl>(std::back_inserter(asdlDelete.targets));
            outer.push(std::move(asdlDelete));
          }
        };
      };
      template <flags::Flag Option>
      struct BreakStmt
          : std::conditional_t<flags::get<Option, flags::LOOP_FLOW>,
                               Break<Option>, failure> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Break{});
          }
        };
      };
      template <flags::Flag Option>
      struct ContinueStmt
          : std::conditional_t<flags::get<Option, flags::LOOP_FLOW>,
                               Continue<Option>, failure> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Continue{});
          }
        };
      };
      template <flags::Flag Option>
      struct ReturnStmt
          : std::conditional_t<flags::get<Option, flags::SCOPE_FLOW>,
                               seq<Return<Option>, opt<TestList<Option>>>,
                               failure> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (has_value()) {
              outer.push(asdl::Return{pop<asdl::ExprImpl>()});
            } else {
              outer.push(asdl::Return{});
            }
          }
        };
      };
      template <flags::Flag Option>
      struct YieldStmt : seq<YieldExpr<Option>> {
        using Transform = rules::ReshapeCapture<asdl::Expr, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct RaiseStmt
          : seq<Raise<Option>,
                opt<Test<Option>, opt<From<Option>, Test<Option>>>> {
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
            outer.push(std::move(raise));
          }
        };
      };
      template <flags::Flag Option>
      struct DottedName : list<Name<Option>, Period<Option>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::vector<asdl::Name> identifiers;
            identifiers.reserve(size());
            transform<asdl::Name>(std::back_inserter(identifiers));
            outer.push(std::accumulate(
                identifiers.begin(), identifiers.end(), asdl::Name{},
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
      };
      template <flags::Flag Option>
      struct DottedAsName
          : seq<DottedName<Option>, opt<As<Option>, Name<Option>>> {
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
      template <flags::Flag Option>
      using DottedAsNames = list<DottedAsName<Option>, Comma<Option>>;
      template <flags::Flag Option>
      struct ImportName : seq<Import<Option>, DottedAsNames<Option>> {
        struct Transform : rules::Stack<asdl::Alias> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Import import;
            import.names.reserve(size());
            transform<asdl::Alias>(std::back_inserter(import.names));
            outer.push(std::move(import));
          }
        };
      };
      template <flags::Flag Option>
      struct ImportFromPeriod : seq<token::PeriodImpl<Option>> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.template top<asdl::ImportFrom>().module.value.append(1, '.');
          }
        };
      };
      template <flags::Flag Option>
      struct ImportFromModuleName
          : seq<not_at<Import<Option>>, DottedName<Option>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.template top<asdl::ImportFrom>().module.value.append(
                pop<asdl::Name>().value);
          }
        };
      };
      template <flags::Flag Option>
      struct ImportAsName : seq<Name<Option>, opt<As<Option>, Name<Option>>> {
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
      template <flags::Flag Option>
      struct ImportAsNames : list_tail<ImportAsName<Option>, Comma<Option>> {
        struct Transform : rules::Stack<asdl::Alias> {
          template <typename Outer>
          void success(Outer &&outer) {
            [this](auto &&importFrom) {
              importFrom.names.reserve(this->size());
              this->template transform<asdl::Alias>(
                  std::back_inserter(importFrom.names));
            }(outer.template top<asdl::ImportFrom>());
          }
        };
      };
      template <flags::Flag Option>
      struct ImportFrom
          : if_must<
                From<Option>,
                sor<seq<star<ImportFromPeriod<Option>>,
                        ImportFromModuleName<Option>>,
                    plus<token::PeriodImpl<Option>>>,
                Import<Option>,
                sor<std::conditional_t<flags::get<Option, flags::IMPORT_ALL>,
                                       StarOp<Option>, failure>,
                    if_must<LPar<flags::set<Option, flags::IMPLICIT>>,
                            ImportAsNames<flags::set<Option, flags::IMPLICIT>>,
                            RPar<Option>>,
                    ImportAsNames<flags::unSet<Option, flags::ASYNC_FLOW,
                                               flags::SCOPE_FLOW>>>> {
        struct Transform : rules::VariantCapture<asdl::ImportFrom> {
          Transform() { push(asdl::ImportFrom{}); }
        };
      };
      template <flags::Flag Option>
      struct GlobalStmt
          : if_must<Global<Option>, list_must<Name<Option>, Comma<Option>>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Global global;
            global.names.reserve(size());
            transform<asdl::Name>(std::back_inserter(global.names));
            outer.push(std::move(global));
          }
        };
      };
      template <flags::Flag Option>
      struct NonLocalStmt
          : if_must<Nonlocal<Option>, list_must<Name<Option>, Comma<Option>>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::Nonlocal nonlocal;
            nonlocal.names.reserve(size());
            transform<asdl::Name>(std::back_inserter(nonlocal.names));
            outer.push(std::move(nonlocal));
          }
        };
      };
      template <flags::Flag Option>
      struct AssertStmt : if_must<Assert<Option>, Test<Option>,
                                  opt<Comma<Option>, Test<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              outer.push(
                  reshape<asdl::Assert, asdl::ExprImpl, asdl::ExprImpl>());
            } else {
              outer.push(reshape<asdl::Assert, asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct IfStmtBranch : seq<Test<Option>, Suite<Option>> {
        using Transform = rules::ReshapeCapture<asdl::IfBranch, asdl::ExprImpl,
                                                std::vector<asdl::StmtImpl>>;
      };
      template <flags::Flag Option>
      using IfStmtIf = if_must<If<Option>, IfStmtBranch<Option>>;
      template <flags::Flag Option>
      using IfStmtElif = if_must<Elif<Option>, IfStmtBranch<Option>>;
      template <flags::Flag Option>
      struct IfStmtBranches : seq<IfStmtIf<Option>, star<IfStmtElif<Option>>> {
        using Transform = rules::VectorCapture<asdl::IfBranch>;
      };
      template <flags::Flag Option>
      using IfStmtElse = if_must<Else<Option>, Suite<Option>>;
      template <flags::Flag Option>
      struct IfStmt : seq<IfStmtBranches<Option>, opt<IfStmtElse<Option>>> {
        struct Transform : rules::Stack<std::vector<asdl::IfBranch>,
                                        std::vector<asdl::StmtImpl>> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              outer.push(reshape<asdl::If, std::vector<asdl::IfBranch>,
                                 std::vector<asdl::StmtImpl>>());
            } else {
              outer.push(reshape<asdl::If, std::vector<asdl::IfBranch>>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct WhileStmt : if_must<While<Option>, Test<Option>,
                                 Suite<flags::set<Option, flags::LOOP_FLOW>>,
                                 opt<Else<Option>, must<Suite<Option>>>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 2) {
              outer.push(reshape<asdl::While, asdl::ExprImpl,
                                 std::vector<asdl::StmtImpl>,
                                 std::vector<asdl::StmtImpl>>());
            } else {
              outer.push(reshape<asdl::While, asdl::ExprImpl,
                                 std::vector<asdl::StmtImpl>>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct ForStmt
          : if_must<For<Option>, ExprList<Option>, In<Option>, TestList<Option>,
                    Suite<flags::set<Option, flags::LOOP_FLOW>>,
                    opt<Else<Option>, must<Suite<Option>>>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 3) {
              outer.push(reshape<asdl::For, asdl::ExprImpl, asdl::ExprImpl,
                                 std::vector<asdl::StmtImpl>,
                                 std::vector<asdl::StmtImpl>>());
            } else {
              outer.push(reshape<asdl::For, asdl::ExprImpl, asdl::ExprImpl,
                                 std::vector<asdl::StmtImpl>>());
            }
          }
        };
      };
      template <flags::Flag Option>
      using TryStmtTry = if_must<Try<Option>, Suite<Option>>;
      template <flags::Flag Option>
      using ExceptClause =
          seq<Except<Option>,
              opt<Test<Option>, opt<As<Option>, must<Name<Option>>>>>;
      template <flags::Flag Option>
      struct TryStmtExcept : if_must<ExceptClause<Option>, Suite<Option>> {
        struct Transform : rules::Stack<std::vector<asdl::StmtImpl>,
                                        asdl::ExprImpl, asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            switch (size()) {
              case 1:
                outer.push(asdl::ExceptHandler{
                    {}, {}, pop<std::vector<asdl::StmtImpl>>()});
                return;
              case 2:
                break;
              case 3:
                outer.push(reshape<asdl::ExceptHandler, asdl::ExprImpl,
                                   asdl::Name, std::vector<asdl::StmtImpl>>());
                return;
              default:
                Expects(false);
            }
            auto body = pop<std::vector<asdl::StmtImpl>>();
            outer.push(asdl::ExceptHandler{
                pop<asdl::ExprImpl>(), {}, std::move(body)});
          }
        };
      };
      template <flags::Flag Option>
      using TryStmtElse = if_must<Else<Option>, Suite<Option>>;
      template <flags::Flag Option>
      using TryStmtFinally = if_must<Finally<Option>, Suite<Option>>;
      template <flags::Flag Option>
      struct TryStmtExcepts : plus<TryStmtExcept<Option>> {
        using Transform = rules::VectorCapture<asdl::Excepthandler>;
      };
      template <flags::Flag Option>
      struct TryStmt
          : seq<TryStmtTry<Option>,
                sor<seq<TryStmtExcepts<Option>, opt<TryStmtElse<Option>>,
                        opt<TryStmtFinally<Option>>>,
                    TryStmtFinally<Option>>> {
        struct Transform : rules::Stack<std::vector<asdl::StmtImpl>,
                                        std::vector<asdl::Excepthandler>> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(reshape<asdl::Try, std::vector<asdl::StmtImpl>,
                               std::vector<asdl::Excepthandler>>());
          }
        };
      };
      template <flags::Flag Option>
      struct WithItem : seq<Test<Option>, opt<As<Option>, Expr<Option>>> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (size() > 1) {
              outer.push(
                  reshape<asdl::Withitem, asdl::ExprImpl, asdl::ExprImpl>());
            } else {
              outer.push(reshape<asdl::Withitem, asdl::ExprImpl>());
            }
          }
        };
      };
      template <flags::Flag Option>
      struct WithStmt
          : if_must<With<Option>, list_must<WithItem<Option>, Comma<Option>>,
                    Suite<Option>> {
        struct Transform
            : rules::Stack<std::vector<asdl::StmtImpl>, asdl::Withitem> {
          template <typename Outer>
          void success(Outer &&outer) {
            asdl::With with;
            with.body = pop<std::vector<asdl::StmtImpl>>();
            with.items.reserve(size());
            transform<asdl::Withitem>(std::back_inserter(with.items));
            outer.push(std::move(with));
          }
        };
      };
      template <flags::Flag Option>
      struct ClassDef
          : if_must<Class<Option>, Name<Option>,
                    opt_must<LPar<flags::set<Option, flags::IMPLICIT>>,
                             opt<ArgList<flags::set<Option, flags::IMPLICIT>>>,
                             RPar<Option>>,
                    SuiteWithDoc<flags::unSet<
                        Option, flags::ASYNC_FLOW, flags::IMPORT_ALL,
                        flags::LOOP_FLOW, flags::SCOPE_FLOW>>> {
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
            outer.push(std::move(classDef));
          }
        };
      };
      template <flags::Flag Option>
      using FlowStmt =
          sor<BreakStmt<Option>, ContinueStmt<Option>, ReturnStmt<Option>,
              RaiseStmt<Option>, YieldStmt<Option>>;
      template <flags::Flag Option>
      using ImportStmt =
          sor<ImportName<Option>,
              ImportFrom<
                  flags::unSet<Option, flags::ASYNC_FLOW, flags::SCOPE_FLOW>>>;
      template <flags::Flag Option>
      using SmallStmt =
          sor<AssertStmt<Option>, DelStmt<Option>, Pass<Option>,
              FlowStmt<Option>, ImportStmt<Option>, GlobalStmt<Option>,
              NonLocalStmt<Option>, ExprStmt<Option>>;
      template <flags::Flag Option>
      using SimpleStmt = if_must<list_tail<SmallStmt<Option>, Semi<Option>>,
                                 sor<NEWLINE, at<Eolf>>>;
      template <flags::Flag Option>
      using Decorator = if_must<AtOp<Option>, Expr<Option>, NEWLINE>;
      template <flags::Flag Option>
      using Decorators = plus<Decorator<Option>>;
      template <flags::Flag Option>
      struct Decorated
          : if_must<Decorators<Option>,
                    sor<ClassDef<Option>,
                        FuncDef<flags::unSet<Option, flags::ASYNC_FLOW>>,
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
      template <flags::Flag Option>
      struct AsyncStmt
          : if_must<Async<Option>,
                    sor<std::conditional_t<
                            flags::get<Option, flags::ASYNC_FLOW>,
                            sor<WithStmt<Option>, ForStmt<Option>>, failure>,
                        FuncDef<flags::set<Option, flags::ASYNC_FLOW>>>> {
        struct Transform
            : rules::Stack<asdl::FunctionDef, asdl::With, asdl::For> {
          struct Push {
            auto operator()(asdl::FunctionDef &&functionDef) {
              return asdl::AsyncFunctionDef{
                  std::move(functionDef.name),
                  functionDef.doc_string,
                  std::move(functionDef.args),
                  std::move(functionDef.body),
                  std::move(functionDef.decorator_list),
                  std::move(functionDef.returns)};
            }
            auto operator()(asdl::With &&with) {
              return asdl::AsyncWith{std::move(with.items),
                                     std::move(with.body)};
            }
            auto operator()(asdl::For &&asdlFor) {
              return asdl::AsyncFor{
                  std::move(asdlFor.target), std::move(asdlFor.iter),
                  std::move(asdlFor.body), std::move(asdlFor.orelse)};
            }
          };
          template <typename Outer>
          void success(Outer &&outer) {
            std::visit(
                [&outer](auto &&value) {
                  outer.push(Push{}(std::forward<decltype(value)>(value)));
                },
                pop());
          }
        };
      };
      template <flags::Flag Option>
      using CompoundStmt =
          sor<IfStmt<Option>, WhileStmt<Option>, ForStmt<Option>,
              TryStmt<Option>, WithStmt<Option>,
              FuncDef<flags::unSet<Option, flags::ASYNC_FLOW>>,
              ClassDef<Option>, Decorated<Option>, AsyncStmt<Option>>;
      template <flags::Flag Option>
      using Stmt = sor<CompoundStmt<Option>, SimpleStmt<Option>>;
      template <flags::Flag Option>
      struct SuiteSimpleStmt : seq<SimpleStmt<Option>> {
        using Transform = rules::VectorCapture<asdl::StmtImpl>;
      };
      template <flags::Flag Option>
      struct SuiteSeqStmtPlus : plus<Stmt<Option>> {
        using Transform = rules::VectorCapture<asdl::StmtImpl>;
      };
      template <flags::Flag Option>
      struct SuiteSeqStmtStar : star<Stmt<Option>> {
        using Transform = rules::VectorCapture<asdl::StmtImpl>;
      };
    } // namespace grammar
  }   // namespace library
} // namespace chimera
