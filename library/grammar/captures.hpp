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

//! generic control status transformations

#pragma once

#include "asdl/asdl.hpp"
#include "grammar/rules/rules.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      using ArgumentsCapture =
          ReshapeCapture<asdl::Arguments, std::vector<Arg>, std::optional<Arg>,
                         std::vector<Arg>, std::optional<Arg>>{};
      using ExcepthandlerCapture = VariantCapture<asdl::ExceptHandler>;
      using OptionalArgCapture = OptionalCapture<asdl::Arg>;
      using OptionalDocStringCapture = OptionalCapture<asdl::DocString>;
      using OptionalExprImplCapture = OptionalCapture<asdl::ExprImpl>;
      using OptionalNameCapture = OptionalCapture<asdl::Name>;
      using SliceImplCapture =
          VariantCapture<asdl::ExtSlice, asdl::Index, asdl::Slice>;
      using VectorAliasCapture = VectorCapture<asdl::Alias>;
      using VectorArgCapture = VectorCapture<asdl::Arg>;
      using VectorCompareExprCapture = VectorCapture<asdl::CompareExpr>;
      using VectorComprehensionCapture = VectorCapture<asdl::Comprehension>;
      using VectorExcepthandlerCapture = VectorCapture<asdl::Excepthandler>;
      using VectorExprImplCapture = VectorCapture<asdl::ExprImpl>;
      using VectorIfBranchCapture = VectorCapture<asdl::IfBranch>;
      using VectorKeywordCapture = VectorCapture<asdl::Keyword>;
      using VectorCapture = VectorCapture<asdl::Name>;
      using VectorSliceImplCapture = VectorCapture<asdl::SliceImpl>;
      using VectorStmtImplCapture = VectorCapture<asdl::StmtImpl>;
      using VectorWithitemCapture = VectorCapture<asdl::Withitem>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
