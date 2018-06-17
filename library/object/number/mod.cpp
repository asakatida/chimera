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
        Real mod(const Left &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return left - rN * floor_div(left * rD, rN) / rD;
              },
              right.numerator, right.denominator);
        }

        template <typename Right>
        Real mod(const Rational &left, const Right &right) {
          return std::visit(
              [&left, &right](const auto &lN, const auto &lD) {
                return Real(left - right * floor_div(lN, lD * right));
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

        Base operator%(std::uint64_t left, const Positive &right) {
          return std::visit([left](const auto &value) { return left % value; },
                            right.value);
        }

        Base operator%(std::uint64_t left, const Negative &right) {
          return std::visit([left](const auto &value) { return left % value; },
                            right.value);
        }

        Base operator%(std::uint64_t left, const Integer &right) {
          return std::visit([left](const auto &value) { return left % value; },
                            right.value);
        }

        Real operator%(std::uint64_t left, const Rational &right) {
          return mod(left, right);
        }

        Real operator%(std::uint64_t left, const Real &right) {
          return std::visit(
              [left](const auto &value) { return Real(left % value); },
              right.value);
        }

        Base operator%(Base left, std::uint64_t right) {
          Expects(right != 0);
          return {left.value % right};
        }

        Base operator%(Base left, Base right) { return left.value % right; }

        Base operator%(Base left, const Natural & /*right*/) { return left; }

        Base operator%(Base left, const Positive &right) {
          return left.value % right;
        }

        Base operator%(Base left, const Negative &right) {
          return left.value % right;
        }

        Base operator%(Base left, const Integer &right) {
          return left.value % right;
        }

        Real operator%(Base left, const Rational &right) {
          return left.value % right;
        }

        Real operator%(Base left, const Real &right) {
          return left.value % right;
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
            return Positive(left);
          }
          if (left == right) {
            return Positive(Base{0u});
          }
          Positive a(left);
          while (right < a) {
            a = +(a - right);
          }
          return a;
        }

        Positive operator%(const Natural &left, const Positive &right) {
          return std::visit(
              [&left](const auto &value) { return Positive(left % value); },
              right.value);
        }

        Positive operator%(const Natural &left, const Negative &right) {
          return std::visit(
              [&left](const auto &value) { return Positive(left % value); },
              right.value);
        }

        Positive operator%(const Natural &left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) { return Positive(left % value); },
              right.value);
        }

        Real operator%(const Natural &left, const Rational &right) {
          return mod(left, right);
        }

        Real operator%(const Natural &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) { return Real(left % value); },
              right.value);
        }

        Positive operator%(const Positive &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Positive(value % right); },
              left.value);
        }

        Positive operator%(const Positive &left, Base right) {
          return left % right.value;
        }

        Positive operator%(const Positive &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value % right); },
              left.value);
        }

        Positive operator%(const Positive &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value % right); },
              left.value);
        }

        Positive operator%(const Positive &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value % right); },
              left.value);
        }

        Positive operator%(const Positive &left, const Integer &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value % right); },
              left.value);
        }

        Real operator%(const Positive &left, const Rational &right) {
          return mod(left, right);
        }

        Real operator%(const Positive &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) { return Real(left % value); },
              right.value);
        }

        Negative operator%(const Negative &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return -(value % right); },
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

        Negative operator%(const Negative &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return -(value % right); },
              left.value);
        }

        Positive operator%(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &a, const auto &b) { return Positive(a % b); },
              left.value, right.value);
        }

        Integer operator%(const Negative &left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) { return Integer(left % value); },
              right.value);
        }

        Real operator%(const Negative &left, const Rational &right) {
          return mod(left, right);
        }

        Real operator%(const Negative &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) { return Real(left % value); },
              right.value);
        }

        Integer operator%(const Integer &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Integer(value % right); },
              left.value);
        }

        Integer operator%(const Integer &left, Base right) {
          return left % right.value;
        }

        Integer operator%(const Integer &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value % right); },
              left.value);
        }

        Integer operator%(const Integer &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value % right); },
              left.value);
        }

        Integer operator%(const Integer &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value % right); },
              left.value);
        }

        Integer operator%(const Integer &left, const Integer &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value % right); },
              left.value);
        }

        Real operator%(const Integer &left, const Rational &right) {
          return mod(left, right);
        }

        Real operator%(const Integer &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) { return Real(left % value); },
              right.value);
        }

        Real operator%(const Rational &left, std::uint64_t right) {
          return mod(left, right);
        }

        Real operator%(const Rational &left, Base right) {
          return left % right.value;
        }

        Real operator%(const Rational &left, const Natural &right) {
          return mod(left, right);
        }

        Real operator%(const Rational &left, const Positive &right) {
          return mod(left, right);
        }

        Real operator%(const Rational &left, const Negative &right) {
          return mod(left, right);
        }

        Real operator%(const Rational &left, const Integer &right) {
          return mod(left, right);
        }

        Real operator%(const Rational &left, const Rational &right) {
          return std::visit(
              [&left](const auto &lN, const auto &lD, const auto &rN,
                      const auto &rD) {
                return left - rN * floor_div(lN * rD, lD * rN) / rD;
              },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }

        Real operator%(const Rational &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) { return Real(left % value); },
              right.value);
        }

        Real operator%(const Real &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Real(value % right); },
              left.value);
        }

        Real operator%(const Real &left, Base right) {
          return std::visit(
              [right](const auto &value) { return Real(value % right); },
              left.value);
        }

        Real operator%(const Real &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Real(value % right); },
              left.value);
        }

        Real operator%(const Real &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return Real(value % right); },
              left.value);
        }

        Real operator%(const Real &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return Real(value % right); },
              left.value);
        }

        Real operator%(const Real &left, const Integer &right) {
          return std::visit(
              [&right](const auto &value) { return Real(value % right); },
              left.value);
        }

        Real operator%(const Real &left, const Rational &right) {
          return std::visit(
              [&right](const auto &value) { return Real(value % right); },
              left.value);
        }

        Real operator%(const Real &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(l % r); },
              left.value, right.value);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
