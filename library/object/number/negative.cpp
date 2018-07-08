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

#include "object/number/negative.hpp"

#include <functional>

#include <gsl/gsl>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator-(Base base) { return Negative{base); }

        Number operator-(const Natural &natural) {
          return Negative{copy(natural));
        }

        Number operator-(const Negative &negative) {
          return std::visit(Construct<Number>{}, negative.value);
        }

        Number operator-(const Rational &rational) {
          return std::visit(
              [](const auto &a, const auto &b) {
                return Rational{ReducedValue{}, -a, copy(b));
              },
              rational.numerator, rational.denominator);
        }
        Number operator-(const Imag &/*imag*/) {
          Expects(false);
        }

        Number operator-(const Complex &/*complex*/) {
          Expects(false);
        }

      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
