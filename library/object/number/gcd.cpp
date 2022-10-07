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

#include "object/number/gcd.hpp"

#include <algorithm>
#include <ranges>

#include <gsl/gsl>

#include "object/number/add.hpp"
#include "object/number/less.hpp"
#include "object/number/mod.hpp"
#include "object/number/mult.hpp"
#include "object/number/negative.hpp"
#include "object/number/overflow.hpp"
#include "object/number/positive.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera::library::object::number {
  template <typename Left>
  auto div(const Left &left, const Rational &right) -> Number {
    return std::visit(
        [&left](auto &&rN, auto &&rD) { return gcd(left * rD, rN); },
        right.numerator, right.denominator);
  }
  template <typename Right>
  auto div(const Rational &left, const Right &right) -> Number {
    return std::visit(
        [&right](auto &&lN, auto &&lD) { return gcd(lN, lD * right); },
        left.numerator, left.denominator);
  }
  auto gcd(std::uint64_t left, Base right) -> Number {
    Number aPrime(left);
    Number bPrime(right);
    while (0U < bPrime) {
      auto temp = bPrime;
      bPrime = aPrime % bPrime;
      aPrime = temp;
    }
    return aPrime;
  }
  auto gcd(std::uint64_t left, const Natural &right) -> Number {
    return gcd(right, left);
  }
  auto gcd(std::uint64_t left, const Negative &right) -> Number {
    return std::visit([left](auto &&value) { return -gcd(left, value); },
                      right.value);
  }
  auto gcd(std::uint64_t left, const Rational &right) -> Number {
    return div(left, right);
  }
  auto gcd(std::uint64_t /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(std::uint64_t /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(Base left, std::uint64_t right) -> Number {
    Expects(right != 0);
    return Number(left.value / right);
  }
  auto gcd(Base left, Base right) -> Number {
    Expects(right.value != 0);
    return Number(left.value / right.value);
  }
  auto gcd(Base left, const Natural &right) -> Number {
    return gcd(left.value, right);
  }
  auto gcd(Base left, const Negative &right) -> Number {
    return gcd(left.value, right);
  }
  auto gcd(Base left, const Rational &right) -> Number {
    return gcd(left.value, right);
  }
  auto gcd(Base /*left*/, const Imag & /*right*/) -> Number { Expects(false); }
  auto gcd(Base /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Natural &left, std::uint64_t right) -> Number {
    Number aPrime(Natural{left});
    Number bPrime(right);
    while (0U < bPrime) {
      auto temp = bPrime;
      bPrime = aPrime % bPrime;
      aPrime = temp;
    }
    return aPrime;
  }
  auto gcd(const Natural &left, Base right) -> Number {
    return gcd(left, right.value);
  }
  auto gcd(const Natural &left, const Natural &right) -> Number {
    Number aPrime(Natural{left});
    Number bPrime(Natural{right});
    while (0U < bPrime) {
      auto temp = bPrime;
      bPrime = aPrime % bPrime;
      aPrime = temp;
    }
    return aPrime;
  }
  auto gcd(const Natural &left, const Negative &right) -> Number {
    return std::visit([&left](auto &&r) { return -gcd(left, r); }, right.value);
  }
  auto gcd(const Natural &left, const Rational &right) -> Number {
    return div(left, right);
  }
  auto gcd(const Natural & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Natural & /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Negative &left, std::uint64_t right) -> Number {
    return std::visit([right](auto &&value) { return -gcd(value, right); },
                      left.value);
  }
  auto gcd(const Negative &left, Base right) -> Number {
    return gcd(left, right.value);
  }
  auto gcd(const Negative &left, const Natural &right) -> Number {
    return std::visit([&right](auto &&value) { return -gcd(value, right); },
                      left.value);
  }
  auto gcd(const Negative &left, const Negative &right) -> Number {
    return std::visit([](auto &&l, auto &&r) { return gcd(l, r); }, left.value,
                      right.value);
  }
  auto gcd(const Negative &left, const Rational &right) -> Number {
    return div(left, right);
  }
  auto gcd(const Negative & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Negative & /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Rational &left, std::uint64_t right) -> Number {
    return div(left, right);
  }
  auto gcd(const Rational &left, Base right) -> Number {
    return gcd(left, right.value);
  }
  auto gcd(const Rational &left, const Natural &right) -> Number {
    return div(left, right);
  }
  auto gcd(const Rational &left, const Negative &right) -> Number {
    return div(left, right);
  }
  auto gcd(const Rational &left, const Rational &right) -> Number {
    return std::visit([](auto &&lN, auto &&lD, auto &&rN,
                         auto &&rD) { return (lN * rD).gcd(lD * rN); },
                      left.numerator, left.denominator, right.numerator,
                      right.denominator);
  }
  auto gcd(const Rational & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Rational & /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Imag & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Imag & /*left*/, Base /*right*/) -> Number { Expects(false); }
  auto gcd(const Imag & /*left*/, const Natural & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Imag & /*left*/, const Negative & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Imag & /*left*/, const Rational & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Imag & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Imag & /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Complex & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Complex & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Complex & /*left*/, const Natural & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Complex & /*left*/, const Negative & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Complex & /*left*/, const Rational & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Complex & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto gcd(const Complex & /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
} // namespace chimera::library::object::number
