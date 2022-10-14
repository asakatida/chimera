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

namespace chimera::library::virtual_machine {
  struct GetEvaluator {
    explicit GetEvaluator(Evaluator *evaluator) noexcept;
    void evaluate(const asdl::Bool &asdlBool) const;
    void evaluate(const asdl::Bin &bin) const;
    void evaluate(const asdl::Unary &unary) const;
    void evaluate(const asdl::Lambda &lambda) const;
    void evaluate(const asdl::IfExp &ifExp) const;
    void evaluate(const asdl::ListComp &listComp) const;
    void evaluate(const asdl::SetComp &setComp) const;
    void evaluate(const asdl::UnpackDict &unpackDict) const;
    void evaluate(const asdl::DictComp &dictComp) const;
    void evaluate(const asdl::GeneratorExp &generatorExp) const;
    void evaluate(const asdl::Await &await) const;
    void evaluate(const asdl::Yield &yield) const;
    void evaluate(const asdl::YieldFrom &yieldFrom) const;
    void evaluate(const asdl::Compare &compare) const;
    void evaluate(const asdl::Call &call) const;
    void evaluate(const asdl::Attribute &attribute) const;
    void evaluate(const asdl::Subscript &subscript) const;
    void evaluate(const asdl::Starred &starred) const;
    void evaluate(const asdl::NameConstant &nameConstant) const;
    void evaluate(const asdl::Ellipsis &ellipsis) const;
    void evaluate(const asdl::FormattedValue &formattedValue) const;
    void evaluate(const asdl::JoinedStr &joinedStr) const;
    void evaluate(const asdl::Name &name) const;
    void evaluate(const asdl::Dict &dict) const;
    void evaluate(const asdl::Set &set) const;
    void evaluate(const asdl::List &list) const;
    void evaluate(const asdl::Tuple &tuple) const;
    void evaluate(const object::Object &object) const;

  private:
    Evaluator *evaluator;
  };
} // namespace chimera::library::virtual_machine
