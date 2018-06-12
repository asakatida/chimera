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
          Colon<flags::mask<Option, flags::IMPLICIT>>,
          sor<DocString<flags::mask<Option, flags::IMPLICIT>>,
              SuiteSimpleStmt<flags::mask<Option, flags::IMPLICIT>>,
              if_must<
                  INDENT<flags::mask<Option, flags::IMPLICIT>>,
                  sor<seq<DocString<flags::mask<Option, flags::IMPLICIT>>,
                          SuiteSeqStmtStar<
                              flags::mask<Option, flags::IMPLICIT>>>,
                      SuiteSeqStmtPlus<flags::mask<Option, flags::IMPLICIT>>>,
                  DEDENT<flags::mask<Option, flags::IMPLICIT>>>>>;
      template <flags::Flag Option>
      using Suite = if_must<
          Colon<flags::mask<Option, flags::IMPLICIT>>,
          sor<SuiteSimpleStmt<flags::mask<Option, flags::IMPLICIT>>,
              if_must<INDENT<flags::mask<Option, flags::IMPLICIT>>,
                      SuiteSeqStmtPlus<flags::mask<Option, flags::IMPLICIT>>,
                      DEDENT<flags::mask<Option, flags::IMPLICIT>>>>>;
      template <flags::Flag Option>
      struct TFPDef
          : seq<Name<flags::mask<Option, flags::IMPLICIT>>,
                opt<Colon<flags::mask<Option, flags::IMPLICIT>>,
                    must<Test<flags::mask<Option, flags::IMPLICIT>>>>> {
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
          : seq<TFPDef<flags::mask<Option, flags::IMPLICIT>>,
                opt<Eq<flags::mask<Option, flags::IMPLICIT>>,
                    must<Test<flags::mask<Option, flags::IMPLICIT>>>>> {
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
          : seq<StarOp<flags::mask<Option, flags::IMPLICIT>>,
                opt<TFPDef<flags::mask<Option, flags::IMPLICIT>>>> {
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
          : if_must<Unpack<flags::mask<Option, flags::IMPLICIT>>,
                    TFPDef<flags::mask<Option, flags::IMPLICIT>>,
                    opt<Comma<flags::mask<Option, flags::IMPLICIT>>>> {
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
          : sor<seq<TypedargsListTFPDefEqTest<
                        flags::mask<Option, flags::IMPLICIT>>,
                    star<Comma<flags::mask<Option, flags::IMPLICIT>>,
                         TypedargsListTFPDefEqTest<
                             flags::mask<Option, flags::IMPLICIT>>>,
                    opt<Comma<flags::mask<Option, flags::IMPLICIT>>,
                        opt<sor<
                            seq<TypedargsListStarTFPDef<
                                    flags::mask<Option, flags::IMPLICIT>>,
                                star<
                                    Comma<flags::mask<Option, flags::IMPLICIT>>,
                                    TypedargsListTFPDefEqTest<
                                        flags::mask<Option, flags::IMPLICIT>>>,
                                opt<Comma<flags::mask<Option, flags::IMPLICIT>>,
                                    opt<TypedargsListUnpackTfpdef<flags::mask<
                                        Option, flags::IMPLICIT>>>>>,
                            TypedargsListUnpackTfpdef<
                                flags::mask<Option, flags::IMPLICIT>>>>>>,
                seq<TypedargsListStarTFPDef<
                        flags::mask<Option, flags::IMPLICIT>>,
                    star<Comma<flags::mask<Option, flags::IMPLICIT>>,
                         TypedargsListTFPDefEqTest<
                             flags::mask<Option, flags::IMPLICIT>>>,
                    opt<Comma<flags::mask<Option, flags::IMPLICIT>>,
                        opt<TypedargsListUnpackTfpdef<
                            flags::mask<Option, flags::IMPLICIT>>>>>,
                TypedargsListUnpackTfpdef<
                    flags::mask<Option, flags::IMPLICIT>>> {};
      template <flags::Flag Option>
      using Parameters =
          if_must<LPar<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                  flags::IMPLICIT>>,
                  opt<TypedArgsList<flags::set<
                      flags::mask<Option, flags::IMPLICIT>, flags::IMPLICIT>>>,
                  RPar<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct FuncDef
          : if_must<
                Def<flags::mask<Option, flags::IMPLICIT>>,
                Name<flags::mask<Option, flags::IMPLICIT>>,
                Parameters<flags::unSet<flags::mask<Option, flags::IMPLICIT>,
                                        flags::ASYNC_FLOW, flags::SCOPE_FLOW>>,
                opt<Arr<flags::mask<Option, flags::IMPLICIT>>,
                    must<Test<
                        flags::unSet<flags::mask<Option, flags::IMPLICIT>,
                                     flags::ASYNC_FLOW, flags::SCOPE_FLOW>>>>,
                SuiteWithDoc<flags::set<
                    flags::unSet<flags::mask<Option, flags::IMPLICIT>,
                                 flags::LOOP_FLOW, flags::IMPORT_ALL>,
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
          : seq<Async<flags::mask<Option, flags::IMPLICIT>>,
                FuncDef<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                   flags::ASYNC_FLOW>>> {
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
      struct ExprStmtTarget
          : seq<TestListStarExpr<flags::mask<Option, flags::IMPLICIT>>> {
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
      using AugAssign = sor<AugAdd<flags::mask<Option, flags::IMPLICIT>>,
                            AugSub<flags::mask<Option, flags::IMPLICIT>>,
                            AugMult<flags::mask<Option, flags::IMPLICIT>>,
                            AugMatMult<flags::mask<Option, flags::IMPLICIT>>,
                            AugDiv<flags::mask<Option, flags::IMPLICIT>>,
                            AugMod<flags::mask<Option, flags::IMPLICIT>>,
                            AugBitAnd<flags::mask<Option, flags::IMPLICIT>>,
                            AugBitOr<flags::mask<Option, flags::IMPLICIT>>,
                            AugBitXor<flags::mask<Option, flags::IMPLICIT>>,
                            AugLShift<flags::mask<Option, flags::IMPLICIT>>,
                            AugRShift<flags::mask<Option, flags::IMPLICIT>>,
                            AugPow<flags::mask<Option, flags::IMPLICIT>>,
                            AugFloorDiv<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct ExprStmtAugAssign
          : if_must<seq<ExprStmtTarget<flags::mask<Option, flags::IMPLICIT>>,
                        AugAssign<flags::mask<Option, flags::IMPLICIT>>>,
                    sor<YieldExpr<flags::mask<Option, flags::IMPLICIT>>,
                        TestList<flags::mask<Option, flags::IMPLICIT>>>> {
        using Transform = rules::ReshapeCapture<asdl::AugAssign, asdl::ExprImpl,
                                                asdl::Operator, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct ExprStmtAssign
          : seq<ExprStmtTarget<flags::mask<Option, flags::IMPLICIT>>,
                Eq<flags::mask<Option, flags::IMPLICIT>>,
                list_must<
                    sor<YieldExpr<flags::mask<Option, flags::IMPLICIT>>,
                        ExprStmtTarget<flags::mask<Option, flags::IMPLICIT>>>,
                    Eq<flags::mask<Option, flags::IMPLICIT>>>> {
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
          : if_must<seq<ExprStmtTarget<flags::mask<Option, flags::IMPLICIT>>,
                        Colon<flags::mask<Option, flags::IMPLICIT>>>,
                    Test<flags::mask<Option, flags::IMPLICIT>>,
                    opt<Eq<flags::mask<Option, flags::IMPLICIT>>,
                        must<Test<flags::mask<Option, flags::IMPLICIT>>>>> {
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
      struct ExprStmtExprStmtTarget
          : seq<ExprStmtTarget<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::ReshapeCapture<asdl::Expr, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      using ExprStmt =
          sor<AnnAssign<flags::mask<Option, flags::IMPLICIT>>,
              ExprStmtAugAssign<flags::mask<Option, flags::IMPLICIT>>,
              ExprStmtAssign<flags::mask<Option, flags::IMPLICIT>>,
              ExprStmtExprStmtTarget<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct DelStmt : if_must<Del<flags::mask<Option, flags::IMPLICIT>>,
                               ExprList<flags::mask<Option, flags::IMPLICIT>>> {
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
                               Break<flags::mask<Option, flags::IMPLICIT>>,
                               failure> {
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
                               Continue<flags::mask<Option, flags::IMPLICIT>>,
                               failure> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(asdl::Continue{});
          }
        };
      };
      template <flags::Flag Option>
      struct ReturnStmt
          : std::conditional_t<
                flags::get<Option, flags::SCOPE_FLOW>,
                seq<Return<flags::mask<Option, flags::IMPLICIT>>,
                    opt<TestList<flags::mask<Option, flags::IMPLICIT>>>>,
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
      struct YieldStmt : seq<YieldExpr<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::ReshapeCapture<asdl::Expr, asdl::ExprImpl>;
      };
      template <flags::Flag Option>
      struct RaiseStmt
          : seq<Raise<flags::mask<Option, flags::IMPLICIT>>,
                opt<Test<flags::mask<Option, flags::IMPLICIT>>,
                    opt<From<flags::mask<Option, flags::IMPLICIT>>,
                        Test<flags::mask<Option, flags::IMPLICIT>>>>> {
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
      struct DottedName : list<Name<flags::mask<Option, flags::IMPLICIT>>,
                               Period<flags::mask<Option, flags::IMPLICIT>>> {
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
          : seq<DottedName<flags::mask<Option, flags::IMPLICIT>>,
                opt<As<flags::mask<Option, flags::IMPLICIT>>,
                    Name<flags::mask<Option, flags::IMPLICIT>>>> {
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
      using DottedAsNames =
          list<DottedAsName<flags::mask<Option, flags::IMPLICIT>>,
               Comma<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct ImportName
          : seq<Import<flags::mask<Option, flags::IMPLICIT>>,
                DottedAsNames<flags::mask<Option, flags::IMPLICIT>>> {
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
      struct ImportFromPeriod
          : seq<token::PeriodImpl<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.template top<asdl::ImportFrom>().module.value.append(1, '.');
          }
        };
      };
      template <flags::Flag Option>
      struct ImportFromModuleName
          : seq<not_at<Import<flags::mask<Option, flags::IMPLICIT>>>,
                DottedName<flags::mask<Option, flags::IMPLICIT>>> {
        struct Transform : rules::Stack<asdl::Name> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.template top<asdl::ImportFrom>().module.value.append(
                pop<asdl::Name>().value);
          }
        };
      };
      template <flags::Flag Option>
      struct ImportAsName
          : seq<Name<flags::mask<Option, flags::IMPLICIT>>,
                opt<As<flags::mask<Option, flags::IMPLICIT>>,
                    Name<flags::mask<Option, flags::IMPLICIT>>>> {
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
      struct ImportAsNames
          : list_tail<ImportAsName<flags::mask<Option, flags::IMPLICIT>>,
                      Comma<flags::mask<Option, flags::IMPLICIT>>> {
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
                From<flags::mask<Option, flags::IMPLICIT>>,
                sor<seq<star<ImportFromPeriod<
                            flags::mask<Option, flags::IMPLICIT>>>,
                        ImportFromModuleName<
                            flags::mask<Option, flags::IMPLICIT>>>,
                    plus<token::PeriodImpl<
                        flags::mask<Option, flags::IMPLICIT>>>>,
                Import<flags::mask<Option, flags::IMPLICIT>>,
                sor<std::conditional_t<
                        flags::get<Option, flags::IMPORT_ALL>,
                        StarOp<flags::mask<Option, flags::IMPLICIT>>, failure>,
                    if_must<
                        LPar<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                        flags::IMPLICIT>>,
                        ImportAsNames<
                            flags::set<flags::mask<Option, flags::IMPLICIT>,
                                       flags::IMPLICIT>>,
                        RPar<flags::mask<Option, flags::IMPLICIT>>>,
                    ImportAsNames<
                        flags::unSet<flags::mask<Option, flags::IMPLICIT>,
                                     flags::ASYNC_FLOW, flags::SCOPE_FLOW>>>> {
        struct Transform : rules::VariantCapture<asdl::ImportFrom> {
          Transform() { push(asdl::ImportFrom{}); }
        };
      };
      template <flags::Flag Option>
      struct GlobalStmt
          : if_must<Global<flags::mask<Option, flags::IMPLICIT>>,
                    list_must<Name<flags::mask<Option, flags::IMPLICIT>>,
                              Comma<flags::mask<Option, flags::IMPLICIT>>>> {
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
          : if_must<Nonlocal<flags::mask<Option, flags::IMPLICIT>>,
                    list_must<Name<flags::mask<Option, flags::IMPLICIT>>,
                              Comma<flags::mask<Option, flags::IMPLICIT>>>> {
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
      struct AssertStmt
          : if_must<Assert<flags::mask<Option, flags::IMPLICIT>>,
                    Test<flags::mask<Option, flags::IMPLICIT>>,
                    opt<Comma<flags::mask<Option, flags::IMPLICIT>>,
                        Test<flags::mask<Option, flags::IMPLICIT>>>> {
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
      struct IfStmtBranch : seq<Test<flags::mask<Option, flags::IMPLICIT>>,
                                Suite<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::ReshapeCapture<asdl::IfBranch, asdl::ExprImpl,
                                                std::vector<asdl::StmtImpl>>;
      };
      template <flags::Flag Option>
      using IfStmtIf =
          if_must<If<flags::mask<Option, flags::IMPLICIT>>,
                  IfStmtBranch<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using IfStmtElif =
          if_must<Elif<flags::mask<Option, flags::IMPLICIT>>,
                  IfStmtBranch<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct IfStmtBranches
          : seq<IfStmtIf<flags::mask<Option, flags::IMPLICIT>>,
                star<IfStmtElif<flags::mask<Option, flags::IMPLICIT>>>> {
        using Transform = rules::VectorCapture<asdl::IfBranch>;
      };
      template <flags::Flag Option>
      using IfStmtElse = if_must<Else<flags::mask<Option, flags::IMPLICIT>>,
                                 Suite<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct IfStmt
          : seq<IfStmtBranches<flags::mask<Option, flags::IMPLICIT>>,
                opt<IfStmtElse<flags::mask<Option, flags::IMPLICIT>>>> {
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
      struct WhileStmt
          : if_must<While<flags::mask<Option, flags::IMPLICIT>>,
                    Test<flags::mask<Option, flags::IMPLICIT>>,
                    Suite<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                     flags::LOOP_FLOW>>,
                    opt<Else<flags::mask<Option, flags::IMPLICIT>>,
                        must<Suite<flags::mask<Option, flags::IMPLICIT>>>>> {
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
          : if_must<For<flags::mask<Option, flags::IMPLICIT>>,
                    ExprList<flags::mask<Option, flags::IMPLICIT>>,
                    In<flags::mask<Option, flags::IMPLICIT>>,
                    TestList<flags::mask<Option, flags::IMPLICIT>>,
                    Suite<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                     flags::LOOP_FLOW>>,
                    opt<Else<flags::mask<Option, flags::IMPLICIT>>,
                        must<Suite<flags::mask<Option, flags::IMPLICIT>>>>> {
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
      using TryStmtTry = if_must<Try<flags::mask<Option, flags::IMPLICIT>>,
                                 Suite<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using ExceptClause =
          seq<Except<flags::mask<Option, flags::IMPLICIT>>,
              opt<Test<flags::mask<Option, flags::IMPLICIT>>,
                  opt<As<flags::mask<Option, flags::IMPLICIT>>,
                      must<Name<flags::mask<Option, flags::IMPLICIT>>>>>>;
      template <flags::Flag Option>
      struct TryStmtExcept
          : if_must<ExceptClause<flags::mask<Option, flags::IMPLICIT>>,
                    Suite<flags::mask<Option, flags::IMPLICIT>>> {
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
      using TryStmtElse = if_must<Else<flags::mask<Option, flags::IMPLICIT>>,
                                  Suite<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using TryStmtFinally =
          if_must<Finally<flags::mask<Option, flags::IMPLICIT>>,
                  Suite<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct TryStmtExcepts
          : plus<TryStmtExcept<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::VectorCapture<asdl::Excepthandler>;
      };
      template <flags::Flag Option>
      struct TryStmt
          : seq<TryStmtTry<flags::mask<Option, flags::IMPLICIT>>,
                sor<seq<TryStmtExcepts<flags::mask<Option, flags::IMPLICIT>>,
                        opt<TryStmtElse<flags::mask<Option, flags::IMPLICIT>>>,
                        opt<TryStmtFinally<
                            flags::mask<Option, flags::IMPLICIT>>>>,
                    TryStmtFinally<flags::mask<Option, flags::IMPLICIT>>>> {
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
      struct WithItem : seq<Test<flags::mask<Option, flags::IMPLICIT>>,
                            opt<As<flags::mask<Option, flags::IMPLICIT>>,
                                Expr<flags::mask<Option, flags::IMPLICIT>>>> {
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
          : if_must<With<flags::mask<Option, flags::IMPLICIT>>,
                    list_must<WithItem<flags::mask<Option, flags::IMPLICIT>>,
                              Comma<flags::mask<Option, flags::IMPLICIT>>>,
                    Suite<flags::mask<Option, flags::IMPLICIT>>> {
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
          : if_must<
                Class<flags::mask<Option, flags::IMPLICIT>>,
                Name<flags::mask<Option, flags::IMPLICIT>>,
                opt_must<
                    LPar<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                    flags::IMPLICIT>>,
                    opt<ArgList<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                           flags::IMPLICIT>>>,
                    RPar<flags::mask<Option, flags::IMPLICIT>>>,
                SuiteWithDoc<flags::unSet<
                    flags::mask<Option, flags::IMPLICIT>, flags::ASYNC_FLOW,
                    flags::IMPORT_ALL, flags::LOOP_FLOW, flags::SCOPE_FLOW>>> {
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
      using FlowStmt = sor<BreakStmt<flags::mask<Option, flags::IMPLICIT>>,
                           ContinueStmt<flags::mask<Option, flags::IMPLICIT>>,
                           ReturnStmt<flags::mask<Option, flags::IMPLICIT>>,
                           RaiseStmt<flags::mask<Option, flags::IMPLICIT>>,
                           YieldStmt<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using ImportStmt =
          sor<ImportName<flags::mask<Option, flags::IMPLICIT>>,
              ImportFrom<flags::unSet<flags::mask<Option, flags::IMPLICIT>,
                                      flags::ASYNC_FLOW, flags::SCOPE_FLOW>>>;
      template <flags::Flag Option>
      using SmallStmt = sor<AssertStmt<flags::mask<Option, flags::IMPLICIT>>,
                            DelStmt<flags::mask<Option, flags::IMPLICIT>>,
                            Pass<flags::mask<Option, flags::IMPLICIT>>,
                            FlowStmt<flags::mask<Option, flags::IMPLICIT>>,
                            ImportStmt<flags::mask<Option, flags::IMPLICIT>>,
                            GlobalStmt<flags::mask<Option, flags::IMPLICIT>>,
                            NonLocalStmt<flags::mask<Option, flags::IMPLICIT>>,
                            ExprStmt<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using SimpleStmt =
          if_must<list_tail<SmallStmt<flags::mask<Option, flags::IMPLICIT>>,
                            Semi<flags::mask<Option, flags::IMPLICIT>>>,
                  sor<NEWLINE<flags::mask<Option, flags::IMPLICIT>>, at<Eolf>>>;
      template <flags::Flag Option>
      using Decorator = if_must<AtOp<flags::mask<Option, flags::IMPLICIT>>,
                                Expr<flags::mask<Option, flags::IMPLICIT>>,
                                NEWLINE<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using Decorators = plus<Decorator<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct Decorated
          : if_must<
                Decorators<flags::mask<Option, flags::IMPLICIT>>,
                sor<ClassDef<flags::mask<Option, flags::IMPLICIT>>,
                    FuncDef<flags::unSet<flags::mask<Option, flags::IMPLICIT>,
                                         flags::ASYNC_FLOW>>,
                    AsyncFuncDef<flags::mask<Option, flags::IMPLICIT>>>> {
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
          : if_must<Async<flags::mask<Option, flags::IMPLICIT>>,
                    sor<std::conditional_t<
                            flags::get<Option, flags::ASYNC_FLOW>,
                            sor<WithStmt<flags::mask<Option, flags::IMPLICIT>>,
                                ForStmt<flags::mask<Option, flags::IMPLICIT>>>,
                            failure>,
                        FuncDef<flags::set<flags::mask<Option, flags::IMPLICIT>,
                                           flags::ASYNC_FLOW>>>> {
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
          sor<IfStmt<flags::mask<Option, flags::IMPLICIT>>,
              WhileStmt<flags::mask<Option, flags::IMPLICIT>>,
              ForStmt<flags::mask<Option, flags::IMPLICIT>>,
              TryStmt<flags::mask<Option, flags::IMPLICIT>>,
              WithStmt<flags::mask<Option, flags::IMPLICIT>>,
              FuncDef<flags::unSet<flags::mask<Option, flags::IMPLICIT>,
                                   flags::ASYNC_FLOW>>,
              ClassDef<flags::mask<Option, flags::IMPLICIT>>,
              Decorated<flags::mask<Option, flags::IMPLICIT>>,
              AsyncStmt<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      using Stmt = sor<CompoundStmt<flags::mask<Option, flags::IMPLICIT>>,
                       SimpleStmt<flags::mask<Option, flags::IMPLICIT>>>;
      template <flags::Flag Option>
      struct SuiteSimpleStmt
          : seq<SimpleStmt<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::VectorCapture<asdl::StmtImpl>;
      };
      template <flags::Flag Option>
      struct SuiteSeqStmtPlus
          : plus<Stmt<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::VectorCapture<asdl::StmtImpl>;
      };
      template <flags::Flag Option>
      struct SuiteSeqStmtStar
          : star<Stmt<flags::mask<Option, flags::IMPLICIT>>> {
        using Transform = rules::VectorCapture<asdl::StmtImpl>;
      };
    } // namespace grammar
  }   // namespace library
} // namespace chimera
