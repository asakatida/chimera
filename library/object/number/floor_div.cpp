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

#include "object/number/floor_div.hpp"

#include <algorithm>

#include <gsl/gsl>

#include "container/reverse.hpp"
#include "object/number/add.hpp"
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
        template <typename T>
        T copy(const T &t) {
          return t;
        }

        template <typename Left>
        Integer div(const Left &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return Integer(floor_div(left * rD, rN));
              },
              right.numerator, right.denominator);
        }

        template <typename Right>
        Integer div(const Rational &left, const Right &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Integer(floor_div(lN, lD * right));
              },
              left.numerator, left.denominator);
        }

        Base floor_div(std::uint64_t left, Base right) {
          Expects(right.value != 0);
          return Base{left / right.value};
        }

        Base floor_div(std::uint64_t /*left*/, const Natural & /*right*/) {
          return Base{0u};
        }

        Base floor_div(std::uint64_t left, const Positive &right) {
          return std::visit(
              [left](const auto &value) { return floor_div(left, value); },
              right.value);
        }

        Negative floor_div(std::uint64_t left, const Negative &right) {
          return std::visit(
              [left](const auto &value) { return -floor_div(left, value); },
              right.value);
        }

        Integer floor_div(std::uint64_t left, const Integer &right) {
          return std::visit(
              [left](const auto &value) {
                return Integer(floor_div(left, value));
              },
              right.value);
        }

        Integer floor_div(std::uint64_t left, const Rational &right) {
          return div(left, right);
        }

        Integer floor_div(std::uint64_t left, const Real &right) {
          return std::visit(
              [left](const auto &value) {
                return Integer(floor_div(left, value));
              },
              right.value);
        }

        Base floor_div(Base left, std::uint64_t right) {
          Expects(right != 0);
          return {left.value / right};
        }

        Base floor_div(Base left, Base right) {
          Expects(right.value != 0);
          return {left.value / right.value};
        }

        Base floor_div(Base left, const Natural &right) {
          return floor_div(left.value, right);
        }

        Base floor_div(Base left, const Positive &right) {
          return floor_div(left.value, right);
        }

        Negative floor_div(Base left, const Negative &right) {
          return floor_div(left.value, right);
        }

        Integer floor_div(Base left, const Integer &right) {
          return floor_div(left.value, right);
        }

        Integer floor_div(Base left, const Rational &right) {
          return floor_div(left.value, right);
        }

        Integer floor_div(Base left, const Real &right) {
          return floor_div(left.value, right);
        }

        Positive floor_div(const Natural &left, std::uint64_t right) {
          Expects(right != 0);
          Natural value;
          Carryover carryover{};
          for (std::uint64_t i : container::reverse(left.value)) {
            carryover.result = i;
            carryover = div(carryover, right);
            value.value.push_back(carryover.result);
          }
          std::reverse(value.value.begin(), value.value.end());
          return Positive(std::move(value));
        }

        Positive floor_div(const Natural &left, Base right) {
          return floor_div(left, right.value);
        }

        Positive floor_div(const Natural &left, const Natural &right) {
          if (left < right) {
            return Positive(Base{0u});
          }
          if (!(right < left)) {
            return Positive(Base{1u});
          }
          Positive a(copy(left));
          Positive b(Base{0u});
          while (right < a) {
            a = +(a - right);
            b = b + 1;
          }
          return b;
        }

        Positive floor_div(const Natural &left, const Positive &right) {
          return std::visit(
              [&left](const auto &value) {
                return Positive(floor_div(left, value));
              },
              right.value);
        }

        Negative floor_div(const Natural &left, const Negative &right) {
          return std::visit(
              [&left](const auto &r) { return -floor_div(left, r); },
              right.value);
        }

        Integer floor_div(const Natural &left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) {
                return Integer(floor_div(left, value));
              },
              right.value);
        }

        Integer floor_div(const Natural &left, const Rational &right) {
          return div(left, right);
        }

        Integer floor_div(const Natural &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) {
                return Integer(floor_div(left, value));
              },
              right.value);
        }

        Positive floor_div(const Positive &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) {
                return Positive(floor_div(value, right));
              },
              left.value);
        }

        Positive floor_div(const Positive &left, Base right) {
          return floor_div(left, right.value);
        }

        Positive floor_div(const Positive &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) {
                return Positive(floor_div(value, right));
              },
              left.value);
        }

        Positive floor_div(const Positive &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Positive(floor_div(l, r));
              },
              left.value, right.value);
        }

        Negative floor_div(const Positive &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return -floor_div(l, r); },
              left.value, right.value);
        }

        Integer floor_div(const Positive &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(floor_div(l, r));
              },
              left.value, right.value);
        }

        Integer floor_div(const Positive &left, const Rational &right) {
          return div(left, right);
        }

        Integer floor_div(const Positive &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(floor_div(l, r));
              },
              left.value, right.value);
        }

        Negative floor_div(const Negative &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return -floor_div(value, right); },
              left.value);
        }

        Negative floor_div(const Negative &left, Base right) {
          return floor_div(left, right.value);
        }

        Negative floor_div(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return -floor_div(value, right); },
              left.value);
        }

        Negative floor_div(const Negative &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return -floor_div(l, r); },
              left.value, right.value);
        }

        Positive floor_div(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Positive(floor_div(l, r));
              },
              left.value, right.value);
        }

        Integer floor_div(const Negative &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(-floor_div(l, r));
              },
              left.value, right.value);
        }

        Integer floor_div(const Negative &left, const Rational &right) {
          return div(left, right);
        }

        Integer floor_div(const Negative &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(-floor_div(l, r));
              },
              left.value, right.value);
        }

        Integer floor_div(const Integer &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) {
                return Integer(floor_div(value, right));
              },
              left.value);
        }

        Integer floor_div(const Integer &left, Base right) {
          return floor_div(left, right.value);
        }

        Integer floor_div(const Integer &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(floor_div(value, right));
              },
              left.value);
        }

        Integer floor_div(const Integer &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(floor_div(l, r));
              },
              left.value, right.value);
        }

        Integer floor_div(const Integer &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(floor_div(value, right));
              },
              left.value);
        }

        Integer floor_div(const Integer &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(floor_div(l, r));
              },
              left.value, right.value);
        }

        Integer floor_div(const Integer &left, const Rational &right) {
          return div(left, right);
        }

        Integer floor_div(const Integer &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(floor_div(l, r));
              },
              left.value, right.value);
        }

        Integer floor_div(const Rational &left, std::uint64_t right) {
          return div(left, right);
        }

        Integer floor_div(const Rational &left, Base right) {
          return floor_div(left, right.value);
        }

        Integer floor_div(const Rational &left, const Natural &right) {
          return div(left, right);
        }

        Integer floor_div(const Rational &left, const Positive &right) {
          return div(left, right);
        }

        Integer floor_div(const Rational &left, const Negative &right) {
          return div(left, right);
        }

        Integer floor_div(const Rational &left, const Integer &right) {
          return div(left, right);
        }

        Integer floor_div(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) {
                return Integer(floor_div(lN * rD, lD * rN));
              },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }

        Integer floor_div(const Rational &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) {
                return Integer(floor_div(left, value));
              },
              right.value);
        }

        Integer floor_div(const Real &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) {
                return Integer(floor_div(value, right));
              },
              left.value);
        }

        Integer floor_div(const Real &left, Base right) {
          return floor_div(left, right.value);
        }

        Integer floor_div(const Real &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(floor_div(value, right));
              },
              left.value);
        }

        Integer floor_div(const Real &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(floor_div(l, r));
              },
              left.value, right.value);
        }

        Integer floor_div(const Real &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(floor_div(value, right));
              },
              left.value);
        }

        Integer floor_div(const Real &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(floor_div(l, r));
              },
              left.value, right.value);
        }

        Integer floor_div(const Real &left, const Rational &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(floor_div(value, right));
              },
              left.value);
        }

        Integer floor_div(const Real &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(floor_div(l, r));
              },
              left.value, right.value);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
