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

#include "object/number/floor_div.hpp"

#include <gsl/gsl>

#include "container/reverse.hpp"
#include "object/number/compare.hpp"
#include "object/number/less.hpp"
#include "object/number/mult.hpp"
#include "object/number/overflow.hpp"
#include "object/number/simplify.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number floor_div(std::uint64_t left, const Base right) {
          Expects(right.value != 0);
          return Number(Base{left / right.value});
        }

        Number floor_div(std::uint64_t /*left*/, const Natural & /*right*/) {
          return Number();
        }

        Number floor_div(std::uint64_t left, const Integer &right) {
          return -std::visit(
              [&left](const auto &r) { return floor_div(left, r); },
              right.value);
        }

        Number floor_div(std::uint64_t left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD).floor_div(Number(rN));
              },
              right.numerator, right.denominator);
        }

        Number floor_div(const Base left, std::uint64_t right) {
          Expects(right != 0);
          return Number(Base{left.value / right});
        }

        Number floor_div(const Base left, const Base right) {
          Expects(right.value != 0);
          return Number(Base{left.value / right.value});
        }

        Number floor_div(const Base /*left*/, const Natural & /*right*/) {
          return Number();
        }

        Number floor_div(const Base left, const Integer &right) {
          return -std::visit(
              [&left](const auto &r) { return floor_div(left, r); },
              right.value);
        }

        Number floor_div(const Base left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD).floor_div(Number(rN));
              },
              right.numerator, right.denominator);
        }

        Number floor_div(const Natural &left, std::uint64_t right) {
          Expects(right != 0);
          auto value = left;
          Carryover carryover{};
          for (auto &&i : container::reverse(value.value)) {
            carryover.result = i;
            carryover = div(carryover, right);
            i = carryover.result;
          }
          return simplify(value);
        }

        Number floor_div(const Natural &left, const Base right) {
          return floor_div(left, right.value);
        }

        Number floor_div(const Natural &left, const Natural &right) {
          if (left < right) {
            return Number();
          }
          if (left == right) {
            return Number(1u);
          }
          Number a(left);
          while (a > right) {
            a -= right;
          }
          return a;
        }

        Number floor_div(const Natural &left, const Integer &right) {
          return -std::visit(
              [&left](const auto &r) { return floor_div(left, r); },
              right.value);
        }

        Number floor_div(const Natural &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD).floor_div(Number(rN));
              },
              right.numerator, right.denominator);
        }

        Number floor_div(const Integer &left, std::uint64_t right) {
          return -std::visit(
              [&right](const auto &l) { return floor_div(l, right); },
              left.value);
        }

        Number floor_div(const Integer &left, const Base right) {
          return floor_div(left, right.value);
        }

        Number floor_div(const Integer &left, const Natural &right) {
          return -std::visit(
              [&right](const auto &l) { return floor_div(l, right); },
              left.value);
        }

        Number floor_div(const Integer &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return floor_div(l, r); },
              left.value, right.value);
        }

        Number floor_div(const Integer &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD).floor_div(Number(rN));
              },
              right.numerator, right.denominator);
        }

        Number floor_div(const Rational &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Number(lN).floor_div(lD * right);
              },
              left.numerator, left.denominator);
        }

        Number floor_div(const Rational &left, const Base right) {
          return floor_div(left, right.value);
        }

        Number floor_div(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Number(lN).floor_div(lD * right);
              },
              left.numerator, left.denominator);
        }

        Number floor_div(const Rational &left, const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Number(lN).floor_div(lD * right);
              },
              left.numerator, left.denominator);
        }

        Number floor_div(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD).floor_div(lD * rN); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
