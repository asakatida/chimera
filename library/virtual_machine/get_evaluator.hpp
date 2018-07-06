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

//! handles read expressions and pushes their value to the top of the stack

#pragma once

#include "asdl/asdl.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      struct GetEvaluator {
        Evaluator *evaluator;

        void evaluate(const asdl::Bool &asdlBool);
        void evaluate(const asdl::Bin &bin);
        void evaluate(const asdl::Unary &unary);
        void evaluate(const asdl::Lambda &lambda);
        void evaluate(const asdl::IfExp &ifExp);
        void evaluate(const asdl::ListComp &listComp);
        void evaluate(const asdl::SetComp &setComp);
        void evaluate(const asdl::UnpackDict &unpackDict);
        void evaluate(const asdl::DictComp &dictComp);
        void evaluate(const asdl::GeneratorExp &generatorExp);
        void evaluate(const asdl::Await &await);
        void evaluate(const asdl::Yield &yield);
        void evaluate(const asdl::YieldFrom &yieldFrom);
        void evaluate(const asdl::Compare &compare);
        void evaluate(const asdl::Call &call);
        void evaluate(const asdl::Attribute &attribute);
        void evaluate(const asdl::Subscript &subscript);
        void evaluate(const asdl::Starred &starred);
        void evaluate(const asdl::NameConstant &nameConstant);
        void evaluate(const asdl::Ellipsis &ellipsis);
        void evaluate(const asdl::FormattedValue &formattedValue);
        void evaluate(const asdl::JoinedStr &joinedStr);
        void evaluate(const asdl::Name &name);
        void evaluate(const asdl::Dict &dict);
        void evaluate(const asdl::Set &set);
        void evaluate(const asdl::List &list);
        void evaluate(const asdl::Tuple &tuple);
        void evaluate(const object::Object &object);
      };
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
