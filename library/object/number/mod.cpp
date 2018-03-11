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

#include <gsl/gsl>

#include "container/reverse.hpp"
#include "object/number/compare.hpp"
#include "object/number/div.hpp"
#include "object/number/less.hpp"
#include "object/number/mult.hpp"
#include "object/number/overflow.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename Left>
        auto mod(const Left &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return left - rN * (left * rD).floor_div(Number(rN)) / rD;
              },
              right.numerator, right.denominator);
        }
        template <typename Right>
        auto mod(const Rational &left, const Right &right) {
          return std::visit(
              [&left, &right](const auto &lN, const auto &lD) {
                return left - right * Number(lN).floor_div(lD * right);
              },
              left.numerator, left.denominator);
        }

        Number operator%(const std::uint64_t &left, const Base &right) {
          Expects(right.value != 0);
          return Number(Base{left % right.value});
        }

        Number operator%(const std::uint64_t &left, const Natural & /*right*/) {
          return Number(Base{left});
        }

        Number operator%(const std::uint64_t &left, const Integer &right) {
          return std::visit([&left](const auto &value) { return left % value; },
                            right.value);
        }

        Number operator%(const std::uint64_t &left, const Rational &right) {
          return mod(left, right);
        }

        Number operator%(const Base &left, const std::uint64_t &right) {
          Expects(right != 0);
          return Number(Base{left.value % right});
        }

        Number operator%(const Base &left, const Base &right) {
          Expects(right.value != 0);
          return Number(Base{left.value % right.value});
        }

        Number operator%(const Base &left, const Natural & /*right*/) {
          return Number(left);
        }

        Number operator%(const Base &left, const Integer &right) {
          return std::visit([&left](const auto &value) { return left % value; },
                            right.value);
        }

        Number operator%(const Base &left, const Rational &right) {
          return mod(left, right);
        }

        Number operator%(const Natural &left, const std::uint64_t &right) {
          Expects(right != 0);
          Carryover carryover{};
          for (auto &&i : container::reverse(left.value)) {
            carryover.result = i;
            carryover = div(carryover, right);
          }
          return Number(Base{carryover.overflow});
        }

        Number operator%(const Natural &left, const Base &right) {
          Expects(right.value != 0);
          Carryover carryover{};
          for (auto &&i : container::reverse(left.value)) {
            carryover.result = i;
            carryover = div(carryover, right.value);
          }
          return Number(Base{carryover.overflow});
        }

        Number operator%(const Natural &left, const Natural &right) {
          if (left < right) {
            return Number(left);
          }
          if (left == right) {
            return Number();
          }
          Number a(left);
          while (a > right) {
            a -= right;
          }
          return a;
        }

        Number operator%(const Natural &left, const Integer &right) {
          return std::visit([&left](const auto &value) { return left % value; },
                            right.value);
        }

        Number operator%(const Natural &left, const Rational &right) {
          return mod(left, right);
        }

        Number operator%(const Integer &left, const std::uint64_t &right) {
          return -std::visit(
              [&right](const auto &value) { return value % right; },
              left.value);
        }

        Number operator%(const Integer &left, const Base &right) {
          return -std::visit(
              [&right](const auto &value) { return value % right; },
              left.value);
        }

        Number operator%(const Integer &left, const Natural &right) {
          return -std::visit(
              [&right](const auto &value) { return value % right; },
              left.value);
        }

        Number operator%(const Integer &left, const Integer &right) {
          return std::visit([](const auto &a, const auto &b) { return a % b; },
                            left.value, right.value);
        }

        Number operator%(const Integer &left, const Rational &right) {
          return mod(left, right);
        }

        Number operator%(const Rational &left, const std::uint64_t &right) {
          return mod(left, right);
        }

        Number operator%(const Rational &left, const Base &right) {
          return mod(left, right);
        }

        Number operator%(const Rational &left, const Natural &right) {
          return mod(left, right);
        }

        Number operator%(const Rational &left, const Integer &right) {
          return mod(left, right);
        }

        Number operator%(const Rational &left, const Rational &right) {
          return std::visit(
              [&left](const auto &lN, const auto &lD, const auto &rN,
                      const auto &rD) {
                return left - rN * (lN * rD).floor_div(lD * rN) / rD;
              },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
