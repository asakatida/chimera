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

//! process context attached to a virtual machine thread

#pragma once

#include <iosfwd>
#include <string_view>

#include "asdl/asdl.hpp"
#include "options.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      struct Parse {
        constexpr static auto bufferSize =
            std::numeric_limits<std::uint16_t>::max();

        asdl::Module parse_file(const Options &options,
                                const std::string_view &data,
                                const char *source);

        asdl::Module parse_file(const Options &options, std::istream &input,
                                const char *source);

        asdl::Interactive parse_input(const Options &options,
                                      const std::string_view &data,
                                      const char *source);

        asdl::Interactive parse_input(const Options &options,
                                      std::istream &input, const char *source);
      };
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
