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

//! handles read expressions and pushes their value to the top of the stack

#pragma once

#include "asdl/asdl.hpp" // for ExprImpl (ptr only), StmtImp...
#include "virtual_machine/evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      struct GetEvaluator {
        Evaluator *evaluator;

        void evaluate(const library::asdl::Bool &asdlBool);
        void evaluate(const library::asdl::Bin &bin);
        void evaluate(const library::asdl::Unary &unary);
        void evaluate(const library::asdl::Lambda &lambda);
        void evaluate(const library::asdl::IfExp &ifExp);
        void evaluate(const library::asdl::ListComp &listComp);
        void evaluate(const library::asdl::SetComp &setComp);
        void evaluate(const library::asdl::DictComp &dictComp);
        void evaluate(const library::asdl::GeneratorExp &generatorExp);
        void evaluate(const library::asdl::Await &await);
        void evaluate(const library::asdl::Yield &yield);
        void evaluate(const library::asdl::YieldFrom &yieldFrom);
        void evaluate(const library::asdl::Compare &compare);
        void evaluate(const library::asdl::Call &call);
        void evaluate(const library::asdl::Attribute &attribute);
        void evaluate(const library::asdl::Subscript &subscript);
        void evaluate(const library::asdl::Starred &starred);
        void evaluate(const library::asdl::NameConstant &nameConstant);
        void evaluate(const library::asdl::Ellipsis &ellipsis);
        void evaluate(const library::asdl::FormattedValue &formattedValue);
        void evaluate(const library::asdl::JoinedStr &joinedStr);
        void evaluate(const library::asdl::Name &name);
        void evaluate(const library::asdl::Dict &dict);
        void evaluate(const library::asdl::Set &set);
        void evaluate(const library::asdl::List &list);
        void evaluate(const library::asdl::Tuple &tuple);
        void evaluate(const library::asdl::Constant &constant);
      };
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
