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

#pragma once

#include "grammar/rules.hpp"
#include "grammar/rules/control.hpp"
#include "options.hpp"

#include <gsl/gsl>
#include <tao/pegtl.hpp>

namespace chimera::library::grammar {
  template <typename Grammar, typename... Args>
  void parse(const options::Optimize &optimize, Args &&...args) {
    Ensures((tao::pegtl::parse<tao::pegtl::must<Grammar>, token::Action,
                               typename MakeControl<>::Normal,
                               tao::pegtl::apply_mode::action,
                               tao::pegtl::rewind_mode::required>(args...)));
    switch (optimize) {
      case options::Optimize::NONE:
      case options::Optimize::BASIC:
      case options::Optimize::DISCARD_DOCS:
        break;
    }
  }
} // namespace chimera::library::grammar
