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

#include "virtual_machine/process_context.hpp"

#include <iostream>
#include <string>      // for string
#include <string_view> // for string_view

#include <gsl/gsl>
#include <tao/pegtl.hpp>

#include "asdl/asdl.hpp"
#include "grammar/grammar.hpp"
#include "options.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      static const std::uint16_t BUFFER_SIZE =
          std::numeric_limits<std::uint16_t>::max();

      template <typename... Args>
      void parse(const Options &options, Args &&... args) {
        Ensures(
            (tao::pegtl::parse<grammar::SingleInput<>, grammar::Nothing,
                               grammar::Normal>(std::forward<Args>(args)...)));
        switch (options.optimize) {
          case Optimize::NONE:
            break;
          case Optimize::BASIC:
            break;
          case Optimize::DISCARD_DOCS:
            break;
        }
      }

      asdl::Interactive
      ProcessContext::parse_input(const std::string_view &data,
                                  const char *source) {
        asdl::Interactive interactive;
        parse(global_context.options,
              grammar::Input<tao::pegtl::memory_input<>>(data.data(),
                                                         data.size(), source),
              interactive, *this);
        return interactive;
      }

      asdl::Interactive ProcessContext::parse_input(std::istream &input,
                                                    const char *source) {
        asdl::Interactive interactive;
        parse(global_context.options,
              grammar::Input<tao::pegtl::istream_input<>>(input, BUFFER_SIZE,
                                                          source),
              interactive, *this);
        return interactive;
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
