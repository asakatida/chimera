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

#include <iostream>

#include <tao/pegtl.hpp>

#include "asdl/asdl.hpp"
#include "grammar/grammar.hpp"
#include "virtual_machine/process_context.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      asdl::Module ProcessContext::parse_file(const std::string_view &data,
                                              const char *source) {
        asdl::Module module;
        grammar::parse<grammar::FileInput>(
            global_context.options,
            grammar::Input<tao::pegtl::memory_input<>>(data.data(), data.size(),
                                                       source),
            module, *this);
        return module;
      }

      asdl::Module ProcessContext::parse_file(std::istream &input,
                                              const char *source) {
        asdl::Module module;
        grammar::parse<grammar::FileInput>(
            global_context.options,
            grammar::Input<tao::pegtl::istream_input<>>(input, bufferSize,
                                                        source),
            module, *this);
        return module;
      }

      asdl::Interactive
      ProcessContext::parse_input(const std::string_view &data,
                                  const char *source) {
        asdl::Interactive interactive;
        grammar::parse<grammar::SingleInput>(
            global_context.options,
            grammar::Input<tao::pegtl::memory_input<>>(data.data(), data.size(),
                                                       source),
            interactive, *this);
        return interactive;
      }

      asdl::Interactive ProcessContext::parse_input(std::istream &input,
                                                    const char *source) {
        asdl::Interactive interactive;
        grammar::parse<grammar::SingleInput>(
            global_context.options,
            grammar::Input<tao::pegtl::istream_input<>>(input, bufferSize,
                                                        source),
            interactive, *this);
        return interactive;
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
