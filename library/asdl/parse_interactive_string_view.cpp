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

#include <string>      // for basic_string
#include <string_view> // for basic_string_view, string_view
#include <vector>      // for vector

// NOLINTBEGIN(misc-no-recursion)

#include <tao/pegtl/memory_input.hpp> // for memory_input
#include <tao/pegtl/parse_error.hpp>  // for parse_error

#include "asdl/asdl.hpp"       // for Interactive
#include "grammar/grammar.hpp" // for SingleInput
#include "grammar/input.hpp"   // for Input
#include "grammar/parse.hpp"   // for parse

namespace chimera::library {
  enum class Optimize;
} // namespace chimera::library

namespace chimera::library::asdl {
  Interactive::Interactive(const Optimize &optimize,
                           const std::string_view &data, const char *source) {
    grammar::parse<grammar::SingleInput>(
        optimize,
        grammar::Input<tao::pegtl::memory_input<>>(data.data(), data.size(),
                                                   source),
        *this);
  }
} // namespace chimera::library::asdl

// NOLINTEND(misc-no-recursion)
