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

#include "object/number/div.hpp"

#include <gsl/gsl>

#include "object/number/mult.hpp"
#include "object/number/negative.hpp"
#include "object/number/util.hpp"

// NOLINTBEGIN(readability-identifier-length)

namespace chimera::library::object::number {
  template <typename Left>
  auto div(const Left &left, const Rational &right) -> Number {
    return std::visit(
        [&left](auto &&rN, auto &&rD) { return (left * rD) / rN; },
        right.numerator, right.denominator);
  }
  template <typename Right>
  auto div(const Rational &left, const Right &right) -> Number {
    return std::visit(
        [&right](auto &&lN, auto &&lD) { return lN / (lD * right); },
        left.numerator, left.denominator);
  }
  auto operator/(std::uint64_t left, Base right) -> Number {
    return Number(Rational{Base{left}, right});
  }
  auto operator/(std::uint64_t left, const Natural &right) -> Number {
    return Number(Rational{Base{left}, right});
  }
  auto operator/(std::uint64_t left, const Negative &right) -> Number {
    return std::visit([left](auto &&value) { return -(left / value); },
                      right.value);
  }
  auto operator/(std::uint64_t left, const Rational &right) -> Number {
    return div(left, right);
  }
  auto operator/(std::uint64_t /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(std::uint64_t /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(Base left, std::uint64_t right) -> Number {
    return Number(Rational{left, Base{right}});
  }
  auto operator/(Base left, Base right) -> Number {
    return Number(Rational{left, right});
  }
  auto operator/(Base left, const Natural &right) -> Number {
    return Number(Rational{left, right});
  }
  auto operator/(Base left, const Negative &right) -> Number {
    return std::visit([left](auto &&value) { return -(left / value); },
                      right.value);
  }
  auto operator/(Base left, const Rational &right) -> Number {
    return div(left, right);
  }
  auto operator/(Base /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(Base /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Natural &left, std::uint64_t right) -> Number {
    return Number(Rational{left, Base{right}});
  }
  auto operator/(const Natural &left, Base right) -> Number {
    return Number(Rational{left, right});
  }
  auto operator/(const Natural &left, const Natural &right) -> Number {
    return Number(Rational{left, right});
  }
  auto operator/(const Natural &left, const Negative &right) -> Number {
    return std::visit([&left](auto &&value) { return -(left / value); },
                      right.value);
  }
  auto operator/(const Natural &left, const Rational &right) -> Number {
    return div(left, right);
  }
  auto operator/(const Natural & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Natural & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator/(const Negative &left, std::uint64_t right) -> Number {
    return std::visit([right](auto &&value) { return -(value / right); },
                      left.value);
  }
  auto operator/(const Negative &left, Base right) -> Number {
    return std::visit([right](auto &&value) { return -(value / right); },
                      left.value);
  }
  auto operator/(const Negative &left, const Natural &right) -> Number {
    return std::visit([&right](auto &&value) { return -(value / right); },
                      left.value);
  }
  auto operator/(const Negative &left, const Negative &right) -> Number {
    return std::visit([](auto &&l, auto &&r) { return l / r; }, left.value,
                      right.value);
  }
  auto operator/(const Negative &left, const Rational &right) -> Number {
    return std::visit([&right](auto &&value) { return -div(value, right); },
                      left.value);
  }
  auto operator/(const Negative & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Negative & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator/(const Rational &left, std::uint64_t right) -> Number {
    return div(left, right);
  }
  auto operator/(const Rational &left, Base right) -> Number {
    return div(left, right);
  }
  auto operator/(const Rational &left, const Natural &right) -> Number {
    return div(left, right);
  }
  auto operator/(const Rational &left, const Negative &right) -> Number {
    return div(left, right);
  }
  auto operator/(const Rational &left, const Rational &right) -> Number {
    return std::visit([](auto &&lN, auto &&lD, auto &&rN,
                         auto &&rD) { return (lN * rD) / (lD * rN); },
                      left.numerator, left.denominator, right.numerator,
                      right.denominator);
  }
  auto operator/(const Rational & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Rational & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator/(const Imag & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Imag & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Imag & /*left*/, const Natural & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Imag & /*left*/, const Negative & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Imag & /*left*/, const Rational & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Imag & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Imag & /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Complex & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Complex & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Complex & /*left*/, const Natural & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator/(const Complex & /*left*/, const Negative & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator/(const Complex & /*left*/, const Rational & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator/(const Complex & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator/(const Complex & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
} // namespace chimera::library::object::number

// NOLINTEND(readability-identifier-length)
