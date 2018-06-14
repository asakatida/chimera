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

#pragma once

#include "object/number/details.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename OStream>
        struct Debug {
          OStream &os;

          OStream &operator()(std::uint64_t i) { return os << i; }

          OStream &operator()(Base base) { return os << base.value; }

          OStream &operator()(const Natural &natural) {
            std::for_each(natural.value.begin(), natural.value.end(), *this);
            return os;
          }

          OStream &operator()(const Negative &negative) {
            os << '-';
            return std::visit(*this, negative.value);
          }

          OStream &operator()(const Integer &integer) {
            return std::visit(*this, integer.value);
          }

          OStream &operator()(const Rational &rational) {
            std::visit(*this, rational.numerator);
            os << '/';
            return std::visit(*this, rational.denominator);
          }
        };
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
