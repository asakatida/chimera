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

//! wrapper for tao::pegtl::input*
//! provides global parse state tracking

#pragma once

#include <cstdint>
#include <stack>

#include <tao/pegtl.hpp>

#include "virtual_machine/process_context.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <typename Base>
      struct Input : Base {
        template <typename Globals, typename... Args>
        explicit Input(Globals &&globals, Args &&... args)
            : Base(std::forward<Args>(args)...), process_context(globals) {
          indentStack.emplace();
        }

        bool indent() {
          std::uintmax_t i = Base::byte_in_line();
          if (indentStack.top() < i) {
            indentStack.push(i);
            return true;
          }
          return false;
        }

        bool is_dedent() const {
          return Base::byte_in_line() < indentStack.top();
        }

        bool dedent() {
          indentStack.pop();
          if (Base::empty()) {
            return true;
          }
          std::uintmax_t i = Base::byte_in_line();
          if (i > indentStack.top()) {
            throw tao::pegtl::parse_error("bad dedent", *this);
          }
          return i == indentStack.top();
        }

        bool is_newline() const {
          return Base::byte_in_line() == indentStack.top();
        }

        virtual_machine::ProcessContext &process_context;

      private:
        std::stack<std::uintmax_t> indentStack{};
      };
    } // namespace grammar
  }   // namespace library
} // namespace chimera
