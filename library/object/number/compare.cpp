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

#include "object/number/compare.hpp"

#include <functional>

#include <gsl/gsl>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        bool operator==(std::uint64_t left, Base right) {
          return left == right.value;
        }
        bool operator==(std::uint64_t /*left*/, const Natural & /*right*/) {
          return false;
        }
        bool operator==(std::uint64_t /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        bool operator==(std::uint64_t /*left*/, const Negative & /*right*/) {
          return false;
        }
        bool operator==(std::uint64_t /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        bool operator==(std::uint64_t /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator==(Base left, std::uint64_t right) {
          return left.value == right;
        }
        bool operator==(Base left, Base right) {
          return left.value == right.value;
        }
        bool operator==(Base /*left*/, const Natural & /*right*/) {
          return false;
        }
        bool operator==(Base /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        bool operator==(Base /*left*/, const Negative & /*right*/) {
          return false;
        }
        bool operator==(Base /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        bool operator==(Base /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator==(const Natural & /*left*/, std::uint64_t /*right*/) {
          return false;
        }
        bool operator==(const Natural & /*left*/, Base /*right*/) {
          return false;
        }
        bool operator==(const Natural &left, const Natural &right) {
          if (left.value.size() != right.value.size()) {
            return false;
          }
          return std::equal(left.value.begin(), left.value.end(),
                            right.value.begin());
        }
        bool operator==(const Natural & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        bool operator==(const Natural & /*left*/, const Negative & /*right*/) {
          return false;
        }
        bool operator==(const Natural & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        bool operator==(const Natural & /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator==(const Positive & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }
        bool operator==(const Positive & /*left*/, Base /*right*/) {
          Expects(false);
        }
        bool operator==(const Positive & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }
        bool operator==(const Positive & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        bool operator==(const Positive & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }
        bool operator==(const Positive & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        bool operator==(const Positive & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }
        bool operator==(const Negative & /*left*/, std::uint64_t /*right*/) {
          return false;
        }
        bool operator==(const Negative & /*left*/, Base /*right*/) {
          return false;
        }
        bool operator==(const Negative & /*left*/, const Natural & /*right*/) {
          return false;
        }
        bool operator==(const Negative & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        bool operator==(const Negative &left, const Negative &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }
        bool operator==(const Negative & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        bool operator==(const Negative & /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator==(const Integer & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }
        bool operator==(const Integer & /*left*/, Base /*right*/) {
          Expects(false);
        }
        bool operator==(const Integer & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }
        bool operator==(const Integer & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        bool operator==(const Integer & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }
        bool operator==(const Integer & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        bool operator==(const Integer & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }
        bool operator==(const Rational & /*left*/, std::uint64_t /*right*/) {
          return false;
        }
        bool operator==(const Rational & /*left*/, Base /*right*/) {
          return false;
        }
        bool operator==(const Rational & /*left*/, const Natural & /*right*/) {
          return false;
        }
        bool operator==(const Rational & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        bool operator==(const Rational & /*left*/, const Negative & /*right*/) {
          return false;
        }
        bool operator==(const Rational & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        bool operator==(const Rational &left, const Rational &right) {
          return std::visit(std::equal_to<>{}, left.numerator,
                            right.numerator) &&
                 std::visit(std::equal_to<>{}, left.denominator,
                            right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
