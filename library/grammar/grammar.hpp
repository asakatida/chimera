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
      template <bool Implicit = false, bool AsyncFlow = false,
                bool LoopFlow = false, bool ScopeFlow = false,
                bool ImportAll = true>
      struct SingleInputImpl
          : Sor<NEWLINE,
                IfMust<CompoundStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                    ImportAll>,
                       NEWLINE>,
                Must<SimpleStmt<Implicit, AsyncFlow, LoopFlow, ScopeFlow,
                                ImportAll>>> {
        struct Transform : rules::Stack<asdl::StmtImpl> {
          template <typename Top>
          void success(Top &&top) {
            top.body.reserve(size());
            transform<asdl::StmtImpl>(std::back_inserter(top.body));
          }
        };
      };
      using SingleInput = InputRule<SingleInputImpl<>>;
      template <bool Implicit = false, bool AsyncFlow = false,
                bool LoopFlow = false, bool ScopeFlow = false,
                bool ImportAll = true>
      struct FileInputImpl : Must<Opt<NEWLINE>, Opt<DocString<Implicit>>,
                                  Until<Eof, Stmt<Implicit, AsyncFlow, LoopFlow,
                                                  ScopeFlow, ImportAll>>> {
        struct Transform : rules::Stack<asdl::DocString, asdl::StmtImpl> {
          template <typename Top>
          void success(Top &&top) {
            top.body.reserve(size());
            while (top_is<asdl::StmtImpl>()) {
              top.body.push_back(pop<asdl::StmtImpl>());
            }
            std::reverse(top.body.begin(), top.body.end());
            if (has_value()) {
              top.doc_string = pop<asdl::DocString>();
            }
          }
        };
      };
      using FileInput = InputRule<FileInputImpl<>>;
      template <bool Implicit = false, bool AsyncFlow = false,
                bool ScopeFlow = false>
      struct EvalInputImpl
          : Must<TestList<Implicit, AsyncFlow, ScopeFlow>, Opt<NEWLINE>, Eof> {
        struct Transform : rules::Stack<asdl::ExprImpl> {
          template <typename Top>
          void success(Top &&top) {
            if (auto s = size(); s > 1) {
              asdl::Tuple tuple;
              tuple.elts.reserve(s);
              transform<asdl::ExprImpl>(std::back_inserter(tuple.elts));
              top.body = std::move(tuple);
            } else {
              top.body = pop<asdl::ExprImpl>();
            }
          }
        };
      };
      using EvalInput = InputRule<EvalInputImpl<>>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
