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

#include "asdl/asdl.hpp"
#include "grammar/expr.hpp"
#include "grammar/flags.hpp"
#include "grammar/input.hpp"
#include "grammar/parse.hpp"
#include "grammar/rules.hpp"
#include "grammar/stmt.hpp"

#include <algorithm>

namespace chimera::library::grammar {
  constexpr static flags::Flag option = flags::list<flags::IMPORT_ALL>;
  struct SingleInput : seq<opt<token::Token<option, one<';'>>>,
                           sor<NEWLINE, seq<CompoundStmt<option>, NEWLINE>,
                               SimpleStmt<option>>> {
    struct Transform : rules::Stack<asdl::StmtImpl> {
      template <typename Top>
      void success(Top &&top) {
        top.success(*this);
      }
    };
  };
  struct FileInput : seq<star<sor<NEWLINE, DocString<option>,
                                  token::Token<option, one<';'>>>>,
                         until<eof, Stmt<option>>> {
    struct Transform : rules::Stack<asdl::DocString, asdl::StmtImpl> {
      template <typename Top>
      void success(Top &&top) {
        top.success(*this);
      }
    };
  };
  struct EvalInput
      : seq<TestList<flags::list<>>, opt<token::Token<flags::list<>, one<';'>>>,
            opt<NEWLINE>, eof> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Top>
      void success(Top &&top) {
        top.success(*this);
      }
    };
  };
} // namespace chimera::library::grammar
