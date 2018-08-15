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
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename Left>
        Number div(const Left &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return floor_div(left * rD, rN);
              },
              right.numerator, right.denominator);
        }

        template <typename Right>
        Number div(const Rational &left, const Right &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return floor_div(lN, lD * right);
              },
              left.numerator, left.denominator);
        }

        Number floor_div(std::uint64_t left, Base right) {
          Expects(right.value != 0);
          return Number(left / right.value);
        }

        Number floor_div(std::uint64_t /*left*/, const Natural & /*right*/) {
          return Number(0u);
        }

        Number floor_div(std::uint64_t left, const Negative &right) {
          return std::visit(
              [left](const auto &value) { return -floor_div(left, value); },
              right.value);
        }

        Number floor_div(std::uint64_t left, const Rational &right) {
          return div(left, right);
        }

        Number floor_div(std::uint64_t /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number floor_div(std::uint64_t /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number floor_div(Base left, std::uint64_t right) {
          Expects(right != 0);
          return Number(left.value / right);
        }

        Number floor_div(Base left, Base right) {
          Expects(right.value != 0);
          return Number(left.value / right.value);
        }

        Number floor_div(Base left, const Natural &right) {
          return floor_div(left.value, right);
        }

        Number floor_div(Base left, const Negative &right) {
          return floor_div(left.value, right);
        }

        Number floor_div(Base left, const Rational &right) {
          return floor_div(left.value, right);
        }

        Number floor_div(Base /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number floor_div(Base /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Natural &left, std::uint64_t right) {
          Expects(right != 0);
          Natural value;
          Carryover carryover{};
          for (std::uint64_t i : container::reverse(left.value)) {
            carryover.result = i;
            carryover = div_mod(carryover, right);
            value.value.push_back(carryover.result);
          }
          std::reverse(value.value.begin(), value.value.end());
          return Number(std::move(value));
        }

        Number floor_div(const Natural &left, Base right) {
          return floor_div(left, right.value);
        }

        Number floor_div(const Natural &left, const Natural &right) {
          if (left < right) {
            return Number(0u);
          }
          if (!(right < left)) {
            return Number(1u);
          }
          Number a(Natural{left});
          Number b(0u);
          while (right < a) {
            a = +(a - Number(Natural{right}));
            b = b + Number(1u);
          }
          return b;
        }

        Number floor_div(const Natural &left, const Negative &right) {
          return std::visit(
              [&left](const auto &r) { return -floor_div(left, r); },
              right.value);
        }

        Number floor_div(const Natural &left, const Rational &right) {
          return div(left, right);
        }

        Number floor_div(const Natural & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Natural & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Negative &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return -floor_div(value, right); },
              left.value);
        }

        Number floor_div(const Negative &left, Base right) {
          return floor_div(left, right.value);
        }

        Number floor_div(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return -floor_div(value, right); },
              left.value);
        }

        Number floor_div(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return floor_div(l, r); },
              left.value, right.value);
        }

        Number floor_div(const Negative &left, const Rational &right) {
          return div(left, right);
        }

        Number floor_div(const Negative & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Negative & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Rational &left, std::uint64_t right) {
          return div(left, right);
        }

        Number floor_div(const Rational &left, Base right) {
          return floor_div(left, right.value);
        }

        Number floor_div(const Rational &left, const Natural &right) {
          return div(left, right);
        }

        Number floor_div(const Rational &left, const Negative &right) {
          return div(left, right);
        }

        Number floor_div(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD).floor_div(lD * rN); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
        Number floor_div(const Rational & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Rational & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Imag & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Number floor_div(const Imag & /*left*/, Base /*right*/) {
          Expects(false);
        }

        Number floor_div(const Imag & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Imag & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Imag & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Imag & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Imag & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Complex & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Number floor_div(const Complex & /*left*/, Base /*right*/) {
          Expects(false);
        }

        Number floor_div(const Complex & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Complex & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Complex & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Complex & /*left*/, const Imag & /*right*/) {
          Expects(false);
        }

        Number floor_div(const Complex & /*left*/, const Complex & /*right*/) {
          Expects(false);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
