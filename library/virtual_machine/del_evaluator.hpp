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

//! handles individual targets of delete expressions like
//! del a, b, c
//!     ^

#pragma once

#include "asdl/asdl.hpp" // for ExprImpl (ptr only), StmtImp...
#include "virtual_machine/evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      struct DelEvaluator {
        Evaluator *evaluator;

        [[noreturn]] void evaluate(const asdl::Bool &asdlBool);
        [[noreturn]] void evaluate(const asdl::Bin &bin);
        [[noreturn]] void evaluate(const asdl::Unary &unary);
        [[noreturn]] void evaluate(const asdl::Lambda &lambda);
        [[noreturn]] void evaluate(const asdl::IfExp &ifExp);
        [[noreturn]] void evaluate(const asdl::ListComp &listComp);
        [[noreturn]] void evaluate(const asdl::SetComp &setComp);
        [[noreturn]] void evaluate(const asdl::DictComp &dictComp);
        [[noreturn]] void evaluate(const asdl::GeneratorExp &generatorExp);
        [[noreturn]] void evaluate(const asdl::Await &await);
        [[noreturn]] void evaluate(const asdl::Yield &yield);
        [[noreturn]] void evaluate(const asdl::YieldFrom &yieldFrom);
        [[noreturn]] void evaluate(const asdl::Compare &compare);
        [[noreturn]] void evaluate(const asdl::Call &call);
        void evaluate(const asdl::Attribute &attribute);
        void evaluate(const asdl::Subscript &subscript);
        void evaluate(const asdl::Starred &starred);
        [[noreturn]] void evaluate(const asdl::NameConstant &nameConstant);
        [[noreturn]] void evaluate(const asdl::Ellipsis &ellipsis);
        [[noreturn]] void evaluate(const asdl::FormattedValue &formattedValue);
        [[noreturn]] void evaluate(const asdl::JoinedStr &joinedStr);
        void evaluate(const asdl::Name &name);
        [[noreturn]] void evaluate(const asdl::Dict &dict);
        [[noreturn]] void evaluate(const asdl::Set &set);
        [[noreturn]] void evaluate(const asdl::List &list);
        [[noreturn]] void evaluate(const asdl::Tuple &tuple);
        [[noreturn]] void evaluate(const asdl::Constant &constant);
      };
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
