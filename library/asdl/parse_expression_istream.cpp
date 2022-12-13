// Copyright (c) 2022 Asa Katida <github@holomaplefeline.net>
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

//! wrapper for tao::pegtl::parse

#include <iostream>                     // for istream

#include <tao/pegtl/istream_input.hpp>  // for istream_input

#include "asdl/asdl.hpp"                // for Expression
#include "asdl/parse.hpp"               // for bufferSize
#include "grammar/grammar.hpp"          // for EvalInput
#include "grammar/input.hpp"            // for Input
#include "grammar/parse.hpp"            // for parse
#include "options.hpp"

namespace chimera::library::asdl {
  Expression::Expression(const Optimize &optimize, std::istream &&input,
                         const char *source) {
    grammar::parse<grammar::EvalInput>(
        optimize,
        grammar::Input<tao::pegtl::istream_input<>>(input, bufferSize, source),
        *this);
  }
} // namespace chimera::library::asdl
