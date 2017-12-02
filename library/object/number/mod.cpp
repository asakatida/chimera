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

#include "object/number/mod.hpp"

#include <vector>

#include <gsl/gsl>

#include "object/number/add.hpp"
#include "object/number/compare.hpp"
#include "object/number/mult.hpp"
#include "object/number/overflow.hpp"
#include "object/number/positive.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {

      namespace number {
        Number operator%(const std::uint64_t & /*left*/,
                         const Base & /*right*/) {
          return Number{};
        }

        Number operator%(const std::uint64_t & /*left*/,
                         const Natural & /*right*/) {
          return Number{};
        }

        Number operator%(const std::uint64_t & /*left*/,
                         const Integer & /*right*/) {
          return Number{};
        }

        Number operator%(const std::uint64_t & /*left*/,
                         const Rational & /*right*/) {
          return Number{};
        }

        Number operator%(const Base & /*left*/,
                         const std::uint64_t & /*right*/) {
          return Number{};
        }

        Number operator%(const Base &left, const Base &right) {
          return Number{Base{left.value % right.value}};
        }

        Number operator%(const Base & /*left*/, const Natural & /*right*/) {
          return Number{};
        }

        Number operator%(const Base & /*left*/, const Integer & /*right*/) {
          return Number{};
        }

        Number operator%(const Base & /*left*/, const Rational & /*right*/) {
          return Number{};
        }

        Number operator%(const Natural & /*left*/,
                         const std::uint64_t & /*right*/) {
          return Number{};
        }

        Number operator%(const Natural & /*left*/, const Base & /*right*/) {
          return Number{};
        }

        Number operator%(const Natural &left, const Natural &right) {
          auto result = Number{left};
          while (result > right) {
            // auto start = result.value.back().value /
            // right.value.back().value; for (; (right * start) > result;
            // --start) {
            // }
            // result = Number{result} - right * start;
          }
          return Number{result};
        }

        Number operator%(const Natural & /*left*/, const Integer & /*right*/) {
          return Number{};
        }

        Number operator%(const Natural & /*left*/, const Rational & /*right*/) {
          return Number{};
        }

        Number operator%(const Integer & /*left*/,
                         const std::uint64_t & /*right*/) {
          return Number{};
        }

        Number operator%(const Integer & /*left*/, const Base & /*right*/) {
          return Number{};
        }

        Number operator%(const Integer & /*left*/, const Natural & /*right*/) {
          return Number{};
        }

        Number operator%(const Integer &left, const Integer &right) {
          if (left.sign != right.sign) {
            return -std::visit([](auto &&a, auto &&b) { return a % b; },
                               left.value, right.value);
          }
          return std::visit([](auto &&a, auto &&b) { return a % b; },
                            left.value, right.value);
        }

        Number operator%(const Integer & /*left*/, const Rational & /*right*/) {
          return Number{};
        }

        Number operator%(const Rational & /*left*/,
                         const std::uint64_t & /*right*/) {
          return Number{};
        }

        Number operator%(const Rational & /*left*/, const Base & /*right*/) {
          return Number{};
        }

        Number operator%(const Rational & /*left*/, const Natural & /*right*/) {
          return Number{};
        }

        Number operator%(const Rational & /*left*/, const Integer & /*right*/) {
          return Number{};
        }

        Number operator%(const Rational &left, const Rational &right) {
          return std::visit([](auto &&lN, auto &&lD, auto &&rN,
                               auto &&rD) { return (lN * rD) / (lD * rN); },
                            left.numerator, left.denominator, right.numerator,
                            right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
