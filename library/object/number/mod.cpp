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

#include "object/number/mod.hpp"

#include <gsl/gsl>

#include "container/reverse.hpp"
#include "object/number/compare.hpp"
#include "object/number/div.hpp"
#include "object/number/floor_div.hpp"
#include "object/number/less.hpp"
#include "object/number/mult.hpp"
#include "object/number/negative.hpp"
#include "object/number/overflow.hpp"
#include "object/number/positive.hpp"
#include "object/number/sub.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename Left>
        Rational mod(const Left &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return left - rN * floor_div(left * rD, rN) / rD;
              },
              right.numerator, right.denominator);
        }
        template <typename Right>
        Rational mod(const Rational &left, const Right &right) {
          return std::visit(
              [&left, &right](const auto &lN, const auto &lD) {
                return left - right * floor_div(lN, lD * right);
              },
              left.numerator, left.denominator);
        }

        Base operator%(std::uint64_t left, Base right) {
          Expects(right.value != 0);
          return {left % right.value};
        }

        Base operator%(std::uint64_t left, const Natural & /*right*/) {
          return {left};
        }

        Base operator%(std::uint64_t /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        Base operator%(std::uint64_t left, const Negative &right) {
          return std::visit([&left](const auto &value) { return left % value; },
                            right.value);
        }

        Base operator%(std::uint64_t /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        Rational operator%(std::uint64_t left, const Rational &right) {
          return mod(left, right);
        }

        Base operator%(Base left, std::uint64_t right) {
          Expects(right != 0);
          return {left.value % right};
        }

        Base operator%(Base left, Base right) {
          Expects(right.value != 0);
          return {left.value % right.value};
        }

        Base operator%(Base left, const Natural & /*right*/) { return left; }

        Base operator%(Base /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        Base operator%(Base left, const Negative &right) {
          return std::visit([&left](const auto &value) { return left % value; },
                            right.value);
        }

        Base operator%(Base /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        Rational operator%(Base left, const Rational &right) {
          return mod(left, right);
        }

        Base operator%(const Natural &left, std::uint64_t right) {
          Expects(right != 0);
          Carryover carryover{};
          for (std::uint64_t i : container::reverse(left.value)) {
            carryover.result = i;
            carryover = div(carryover, right);
          }
          return {carryover.overflow};
        }

        Base operator%(const Natural &left, Base right) {
          return left % right.value;
        }

        Positive operator%(const Natural &left, const Natural &right) {
          if (left < right) {
            return left;
          }
          if (left == right) {
            return Base{};
          }
          Positive a(left);
          while (right < a) {
            a = +(a - right);
          }
          return a;
        }

        Base operator%(const Natural & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }

        Positive operator%(const Natural &left, const Negative &right) {
          return std::visit(
              [&left](const auto &value) { return Positive(left % value); },
              right.value);
        }

        Base operator%(const Natural & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        Rational operator%(const Natural &left, const Rational &right) {
          return mod(left, right);
        }

        Base operator%(const Positive & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }
        Base operator%(const Positive & /*left*/, Base /*right*/) {
          Expects(false);
        }
        Base operator%(const Positive & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }
        Base operator%(const Positive & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        Base operator%(const Positive & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }
        Base operator%(const Positive & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        Base operator%(const Positive & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }
        Negative operator%(const Negative &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &value) { return -(value % right); },
              left.value);
        }

        Negative operator%(const Negative &left, Base right) {
          return left % right.value;
        }

        Negative operator%(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return -(value % right); },
              left.value);
        }

        Base operator%(const Negative & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        Positive operator%(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &a, const auto &b) { return Positive(a % b); },
              left.value, right.value);
        }

        Base operator%(const Negative & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        Rational operator%(const Negative &left, const Rational &right) {
          return mod(left, right);
        }

        Base operator%(const Integer & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }
        Base operator%(const Integer & /*left*/, Base /*right*/) {
          Expects(false);
        }
        Base operator%(const Integer & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }
        Base operator%(const Integer & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        Base operator%(const Integer & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }
        Base operator%(const Integer & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        Base operator%(const Integer & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }
        Rational operator%(const Rational &left, std::uint64_t right) {
          return mod(left, right);
        }

        Rational operator%(const Rational &left, Base right) {
          return mod(left, right);
        }

        Rational operator%(const Rational &left, const Natural &right) {
          return mod(left, right);
        }

        Base operator%(const Rational & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        Rational operator%(const Rational &left, const Negative &right) {
          return mod(left, right);
        }

        Base operator%(const Rational & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        Rational operator%(const Rational &left, const Rational &right) {
          return std::visit(
              [&left](const auto &lN, const auto &lD, const auto &rN,
                      const auto &rD) {
                return left - rN * floor_div(lN * rD, lD * rN) / rD;
              },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
