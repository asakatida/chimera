// Copyright (c) 2018 Asa Katida <github@holomaplefeline.net>
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

#include "asdl/asdl.hpp"       // for Interactive
#include "grammar/grammar.hpp" // for SingleInput
#include "grammar/input.hpp"   // for Input
#include "grammar/parse.hpp"   // for parse

#include <iostream> // for istream
#include <vector>   // for vector

namespace chimera::library::options {
  enum class Optimize;
} // namespace chimera::library::options

namespace chimera::library::asdl {
  Interactive::Interactive(const options::Optimize &optimize,
                           std::istream &&input, const char *source) {
    grammar::parse<grammar::SingleInput>(
        optimize, grammar::Input(std::move(input), source), *this);
  }
  [[nodiscard]] auto Interactive::iter() const
      -> const std::vector<StmtImpl> & {
    return body;
  }
} // namespace chimera::library::asdl
