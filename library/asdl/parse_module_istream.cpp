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

#include <iostream> // for istream
#include <string>   // for basic_string
#include <vector>   // for vector

// NOLINTBEGIN(misc-no-recursion)

#include <tao/pegtl/istream_input.hpp> // for istream_input
#include <tao/pegtl/parse_error.hpp>   // for parse_error

#include "asdl/asdl.hpp"       // for Module
#include "asdl/parse.hpp"      // for bufferSize
#include "grammar/grammar.hpp" // for FileInput
#include "grammar/input.hpp"   // for Input
#include "grammar/parse.hpp"   // for parse

namespace chimera::library {
  enum class Optimize;
} // namespace chimera::library

namespace chimera::library::asdl {
  Module::Module(const Optimize &optimize, std::istream &&input,
                 const char *source) {
    grammar::parse<grammar::FileInput>(
        optimize,
        grammar::Input<tao::pegtl::istream_input<>>(input, bufferSize, source),
        *this);
  }
  [[nodiscard]] auto Module::doc() const -> const std::optional<DocString> & {
    return doc_string;
  }
  [[nodiscard]] auto Module::iter() const -> const std::vector<StmtImpl> & {
    return body;
  }
} // namespace chimera::library::asdl

// NOLINTEND(misc-no-recursion)
