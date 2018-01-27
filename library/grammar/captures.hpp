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

//! generic control status transformations

#pragma once

#include "asdl/asdl.hpp"
#include "grammar/rules/rules.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      using ArgumentsCapture = rules::ReshapeCapture<
          asdl::Arguments, std::vector<asdl::Arg>, std::optional<asdl::Arg>,
          std::vector<asdl::Arg>, std::optional<asdl::Arg>>;
      using ExcepthandlerCapture = rules::VariantCapture<asdl::ExceptHandler>;
      using OptionalArgCapture = rules::OptionalCapture<asdl::Arg>;
      using OptionalDocStringCapture = rules::OptionalCapture<asdl::DocString>;
      using OptionalExprImplCapture = rules::OptionalCapture<asdl::ExprImpl>;
      using OptionalNameCapture = rules::OptionalCapture<asdl::Name>;
      using SliceImplCapture =
          rules::VariantCapture<asdl::ExtSlice, asdl::Index, asdl::Slice>;
      using VectorAliasCapture = rules::VectorCapture<asdl::Alias>;
      using VectorArgCapture = rules::VectorCapture<asdl::Arg>;
      using VectorCompareExprCapture = rules::VectorCapture<asdl::CompareExpr>;
      using VectorComprehensionCapture =
          rules::VectorCapture<asdl::Comprehension>;
      using VectorExcepthandlerCapture =
          rules::VectorCapture<asdl::Excepthandler>;
      using VectorExprImplCapture = rules::VectorCapture<asdl::ExprImpl>;
      using VectorIfBranchCapture = rules::VectorCapture<asdl::IfBranch>;
      using VectorKeywordCapture = rules::VectorCapture<asdl::Keyword>;
      using VectorCapture = rules::VectorCapture<asdl::Name>;
      using VectorSliceImplCapture = rules::VectorCapture<asdl::SliceImpl>;
      using VectorStmtImplCapture = rules::VectorCapture<asdl::StmtImpl>;
      using VectorWithitemCapture = rules::VectorCapture<asdl::Withitem>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
