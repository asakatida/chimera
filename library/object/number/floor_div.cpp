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
#include <ranges>

#include <gsl/gsl>

#include "object/number/add.hpp"
#include "object/number/less.hpp"
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
        [&left](auto &&rN, auto &&rD) { return floor_div(left * rD, rN); },
        right.numerator, right.denominator);
  }
  template <typename Right>
  auto div(const Rational &left, const Right &right) -> Number {
    return std::visit(
        [&right](auto &&lN, auto &&lD) { return floor_div(lN, lD * right); },
        left.numerator, left.denominator);
  }
  auto floor_div(std::uint64_t left, Base right) -> Number {
    Expects(right.value != 0);
    return Number(left / right.value);
  }
  auto floor_div(std::uint64_t /*left*/, const Natural & /*right*/) -> Number {
    return Number(0U);
  }
  auto floor_div(std::uint64_t left, const Negative &right) -> Number {
    return std::visit([left](auto &&value) { return -floor_div(left, value); },
                      right.value);
  }
  auto floor_div(std::uint64_t left, const Rational &right) -> Number {
    return div(left, right);
  }
  auto floor_div(std::uint64_t /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(std::uint64_t /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(Base left, std::uint64_t right) -> Number {
    Expects(right != 0);
    return Number(left.value / right);
  }
  auto floor_div(Base left, Base right) -> Number {
    Expects(right.value != 0);
    return Number(left.value / right.value);
  }
  auto floor_div(Base left, const Natural &right) -> Number {
    return floor_div(left.value, right);
  }
  auto floor_div(Base left, const Negative &right) -> Number {
    return floor_div(left.value, right);
  }
  auto floor_div(Base left, const Rational &right) -> Number {
    return floor_div(left.value, right);
  }
  auto floor_div(Base /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(Base /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Natural &left, std::uint64_t right) -> Number {
    Expects(right != 0);
    Natural value;
    std::ranges::for_each(left.value | std::views::reverse,
                          [carryover = Carryover{}, &value,
                           right](const std::uint64_t i) mutable {
                            carryover.result = i;
                            carryover = div_mod(carryover, right);
                            value.value.emplace_back(carryover.result);
                          });
    std::reverse(value.value.begin(), value.value.end());
    return Number(std::move(value));
  }
  auto floor_div(const Natural &left, Base right) -> Number {
    return floor_div(left, right.value);
  }
  auto floor_div(const Natural &left, const Natural &right) -> Number {
    if (left < right) {
      return Number(0U);
    }
    if (!(right < left)) {
      return Number(1U);
    }
    Number a(Natural{left});
    Number b(0U);
    while (right < a) {
      a = +(a - Number(Natural{right}));
      b = b + Number(1U);
    }
    return b;
  }
  auto floor_div(const Natural &left, const Negative &right) -> Number {
    return std::visit([&left](auto &&r) { return -floor_div(left, r); },
                      right.value);
  }
  auto floor_div(const Natural &left, const Rational &right) -> Number {
    return div(left, right);
  }
  auto floor_div(const Natural & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Natural & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto floor_div(const Negative &left, std::uint64_t right) -> Number {
    return std::visit(
        [right](auto &&value) { return -floor_div(value, right); }, left.value);
  }
  auto floor_div(const Negative &left, Base right) -> Number {
    return floor_div(left, right.value);
  }
  auto floor_div(const Negative &left, const Natural &right) -> Number {
    return std::visit(
        [&right](auto &&value) { return -floor_div(value, right); },
        left.value);
  }
  auto floor_div(const Negative &left, const Negative &right) -> Number {
    return std::visit([](auto &&l, auto &&r) { return floor_div(l, r); },
                      left.value, right.value);
  }
  auto floor_div(const Negative &left, const Rational &right) -> Number {
    return div(left, right);
  }
  auto floor_div(const Negative & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Negative & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto floor_div(const Rational &left, std::uint64_t right) -> Number {
    return div(left, right);
  }
  auto floor_div(const Rational &left, Base right) -> Number {
    return floor_div(left, right.value);
  }
  auto floor_div(const Rational &left, const Natural &right) -> Number {
    return div(left, right);
  }
  auto floor_div(const Rational &left, const Negative &right) -> Number {
    return div(left, right);
  }
  auto floor_div(const Rational &left, const Rational &right) -> Number {
    return std::visit([](auto &&lN, auto &&lD, auto &&rN,
                         auto &&rD) { return (lN * rD).floor_div(lD * rN); },
                      left.numerator, left.denominator, right.numerator,
                      right.denominator);
  }
  auto floor_div(const Rational & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Rational & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto floor_div(const Imag & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Imag & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Imag & /*left*/, const Natural & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Imag & /*left*/, const Negative & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Imag & /*left*/, const Rational & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Imag & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Imag & /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Complex & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Complex & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Complex & /*left*/, const Natural & /*right*/)
      -> Number {
    Expects(false);
  }
  auto floor_div(const Complex & /*left*/, const Negative & /*right*/)
      -> Number {
    Expects(false);
  }
  auto floor_div(const Complex & /*left*/, const Rational & /*right*/)
      -> Number {
    Expects(false);
  }
  auto floor_div(const Complex & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto floor_div(const Complex & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
} // namespace chimera::library::object::number
