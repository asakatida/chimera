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

//! top level grammars.

#pragma once

#include <algorithm>

#include <tao/pegtl.hpp>

#include "grammar/control.hpp"
#include "grammar/expr.hpp"
#include "grammar/stmt.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      struct SingleInputState : Stack<asdl::StmtImpl> {
        template <typename Top>
        void success(Top &&top) {
          top.body.resize(size());
          transform(top.body.begin());
        }
      };
      template <bool Implicit = false, bool AsyncFlow = false,
                bool LoopFlow = false, bool ScopeFlow = false,
                bool ImportAll = true>
      struct SingleInputImpl
          : tao::pegtl::sor<
                NEWLINE,
                tao::pegtl::if_must<CompoundStmt<Implicit, AsyncFlow, LoopFlow,
                                                 ScopeFlow, ImportAll>,
                                    NEWLINE>,
                tao::pegtl::must<SimpleStmt<Implicit, AsyncFlow, LoopFlow,
                                            ScopeFlow, ImportAll>>> {};
      template <>
      struct Control<SingleInputImpl<>>
          : ChangeState<SingleInputImpl<>, SingleInputState> {};
      struct SingleInput : InputRule<SingleInputImpl<>> {};
      struct FileInputState : Stack<asdl::DocString, asdl::StmtImpl> {
        template <typename Top>
        void success(Top &&top) {
          top.body.resize(size());
          while (top_is<asdl::StmtImpl>()) {
            top.body.push_back(pop<asdl::StmtImpl>());
          }
          std::reverse(top.body.begin(), top.body.end());
          if (has_value()) {
            top.docString = pop<asdl::DocString>();
          }
        }
      };
      template <bool Implicit = false, bool AsyncFlow = false,
                bool LoopFlow = false, bool ScopeFlow = false,
                bool ImportAll = true>
      struct FileInputImpl
          : tao::pegtl::must<
                tao::pegtl::opt<NEWLINE>, tao::pegtl::opt<DocString>,
                tao::pegtl::until<tao::pegtl::eof,
                                  Stmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                       ImportAll>>> {};
      template <>
      struct Control<FileInputImpl<>>
          : ChangeState<FileInputImpl<>, FileInputState> {};
      struct FileInput : InputRule<FileInputImpl<>> {};
      struct EvalInputState : Stack<asdl::ExprImpl> {
        template <typename Top>
        void success(Top &&top) {
          if (auto s = size(); s > 1) {
            asdl::Tuple tuple;
            tuple.elts.resize(s);
            transform(tuple.elts.begin());
            top.body = std::move(tuple);
          } else {
            top.body = pop<asdl::ExprImpl>();
          }
        }
      };
      template <bool Implicit = false, bool AsyncFlow = false,
                bool ScopeFlow = false>
      struct EvalInputImpl
          : tao::pegtl::must<TestList<Implicit, AsyncFlow, ScopeFlow>,
                             tao::pegtl::opt<NEWLINE>, tao::pegtl::eof> {};
      template <>
      struct Control<EvalInputImpl<>>
          : ChangeState<EvalInputImpl<>, EvalInputState> {};
      struct EvalInput : InputRule<EvalInputImpl<>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
