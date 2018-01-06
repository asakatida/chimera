// Copyright (c) 2017 Adam Grandquist <grandquista@gmail.com>
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

//! template options for parse rules.

#pragma once

namespace chimera {
  namespace library {
    namespace grammar {
      template <std::uint8_t Mask = 0>
      struct Options {
        enum {
          ASYNC_FLOW,
          DISCARD,
          IMPLICIT,
          IMPORT_ALL,
          LOOP_FLOW,
          SCOPE_FLOW
        };

        template <int... Bits>
        constexpr static bool get = (Mask & (... | (1 << Bits))) != 0;
        template <int... Bits>
        using Set = Options<(Mask | ... | (1 << Bits))>;
        template <int... Bits>
        using UnSet = Options<(Mask & ... & ~(1 << Bits))>;
      };
    } // namespace grammar
  }   // namespace library
} // namespace chimera
