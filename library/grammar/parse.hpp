// Copyright (c) 2018 Adam Grandquist <grandquista@gmail.com>
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

#pragma once

#include <gsl/gsl> // for Ensures
#include <tao/pegtl.hpp>

#include "grammar/rules.hpp"
#include "options.hpp" // for Optimize, Optimize::B...

namespace chimera {
  namespace library {
    namespace grammar {
      template <typename Grammar, typename... Args>
      void parse(const Options &options, Args &&... args) {
        Ensures((tao::pegtl::parse<Grammar, token::Action, Normal>(args...)));
        switch (options.optimize) {
          case Optimize::NONE:
            break;
          case Optimize::BASIC:
            break;
          case Optimize::DISCARD_DOCS:
            break;
        }
      }
    } // namespace grammar
  }   // namespace library
} // namespace chimera
