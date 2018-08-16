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

#include "object/number/mult.hpp"

#include <gsl/gsl>

#include "object/number/add.hpp"
#include "object/number/left_shift.hpp"
#include "object/number/negative.hpp"
#include "object/number/overflow.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator*(std::uint64_t left, Base right) {
          return right * left;
        }

        Number operator*(std::uint64_t left, const Natural &right) {
          return right * left;
        }

        Number operator*(std::uint64_t left, const Negative &right) {
          return right * left;
        }

        Number operator*(std::uint64_t left, const Rational &right) {
          return right * left;
        }

        Number operator*(std::uint64_t /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number operator*(std::uint64_t /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number operator*(Base left, std::uint64_t right) {
          auto value = mult(left.value, right);
          return Number(Natural{{value.result, value.overflow}});
        }

        Number operator*(Base left, Base right) { return left * right.value; }

        Number operator*(Base left, const Natural &right) {
          return left.value * right;
        }

        Number operator*(Base left, const Negative &right) {
          return left.value * right;
        }

        Number operator*(Base left, const Rational &right) {
          return left.value * right;
        }

        Number operator*(Base /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number operator*(Base /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number operator*(const Natural &left, std::uint64_t right) {
          if (right == 0) {
            return Number(0u);
          }
          if (right == 1) {
            return Number(Natural(left));
          }
          Natural value{};
          value.value.reserve(left.value.size() + 1);
          Carryover carryover{};
          for (std::uint64_t i : left.value) {
            auto m = mult(i, right);
            carryover = sum(m.result, carryover.result);
            value.value.emplace_back(carryover.result);
            carryover = sum(m.overflow, carryover.overflow);
            Ensures(carryover.overflow == 0);
          }
          if (carryover.result != 0) {
            value.value.emplace_back(carryover.result);
          }
          return Number(std::move(value));
        }

        Number operator*(const Natural &left, Base right) {
          return left * right.value;
        }

        Number operator*(const Natural &left, const Natural &right) {
          Number accumulate(0u);
          std::size_t size = 0;
          for (std::uint64_t i : right.value) {
            accumulate = accumulate + ((left * i) << (64 * (size++)));
          }
          return accumulate;
        }

        Number operator*(const Natural &left, const Negative &right) {
          return right * left;
        }

        Number operator*(const Natural &left, const Rational &right) {
          return right * left;
        }

        Number operator*(const Natural & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number operator*(const Natural & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number operator*(const Negative &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return -(value * right); },
              left.value);
        }

        Number operator*(const Negative &left, Base right) {
          return left * right.value;
        }

        Number operator*(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return -(value * right); },
              left.value);
        }

        Number operator*(const Negative &left, const Negative &right) {
          return std::visit([](const auto &l, const auto &r) { return l * r; },
                            left.value, right.value);
        }

        Number operator*(const Negative &left, const Rational &right) {
          return right * left;
        }

        Number operator*(const Negative & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number operator*(const Negative & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number operator*(const Rational &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN * right) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator*(const Rational &left, Base right) {
          return left * right.value;
        }

        Number operator*(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN * right) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator*(const Rational &left, const Negative &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN * right) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator*(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rN) / (lD * rD); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
        Number operator*(const Rational & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number operator*(const Rational & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number operator*(const Imag & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Number operator*(const Imag & /*left*/, Base /*right*/) {
          Expects(false);
        }

        Number operator*(const Imag & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }

        Number operator*(const Imag & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }

        Number operator*(const Imag & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Number operator*(const Imag & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number operator*(const Imag & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number operator*(const Complex & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Number operator*(const Complex & /*left*/, Base /*right*/) {
          Expects(false);
        }

        Number operator*(const Complex & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }

        Number operator*(const Complex & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }

        Number operator*(const Complex & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Number operator*(const Complex & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number operator*(const Complex & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
