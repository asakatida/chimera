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

#include "object/number/right_shift.hpp"

#include <gsl/gsl>

#include "container/reverse.hpp"
#include "object/number/floor_div.hpp"
#include "object/number/left_shift.hpp"
#include "object/number/mod.hpp"
#include "object/number/negative.hpp"
#include "object/number/overflow.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

namespace chimera::library::object::number {
  auto operator>>(std::uint64_t left, Base right) -> Number {
    return Number(left >> right.value);
  }
  auto operator>>(std::uint64_t /*left*/, const Natural & /*right*/) -> Number {
    return Number(0U);
  }
  auto operator>>(std::uint64_t left, const Negative &right) -> Number {
    return std::visit([left](const auto &value) { return left << value; },
                      right.value);
  }
  auto operator>>(std::uint64_t /*left*/, const Rational & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(std::uint64_t /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(std::uint64_t /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(Base left, std::uint64_t right) -> Number {
    return Number(left.value >> right);
  }
  auto operator>>(Base left, Base right) -> Number {
    return Number(left.value >> right.value);
  }
  auto operator>>(Base /*left*/, const Natural & /*right*/) -> Number {
    return Number(0U);
  }
  auto operator>>(Base left, const Negative &right) -> Number {
    return std::visit([left](const auto &value) { return left << value; },
                      right.value);
  }
  auto operator>>(Base /*left*/, const Rational & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(Base /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(Base /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Natural &left, std::uint64_t right) -> Number {
    if (right == 0) {
      return Number(Natural(left));
    }
    if (std::size_t(right / 64) >= left.value.size()) {
      return Number(0U);
    }
    auto value = left;
    // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
    if (const std::ptrdiff_t shift = right / 64; shift != 0) {
      value.value.erase(value.value.begin(), value.value.begin() + shift);
    }
    if (const std::size_t shift = right % 64; shift != 0) {
      Natural result{};
      Carryover carryover{};
      for (const std::uint64_t i : value.value) {
        auto overflow = carryover.overflow;
        carryover = right_shift(i, shift);
        result.value.emplace_back(carryover.result | overflow);
      }
      return Number(std::move(result));
    }
    return Number(std::move(value));
  }
  auto operator>>(const Natural &left, Base right) -> Number {
    return left >> right.value;
  }
  auto operator>>(const Natural &left, const Natural &right) -> Number {
    if (std::size_t(floor_div(right, 64)) >= left.value.size()) {
      return Number(0U);
    }
    auto value = left;
    if (const std::ptrdiff_t shift(floor_div(right, 64)); shift != 0) {
      value.value.erase(value.value.begin(), value.value.begin() + shift);
    }
    if (const std::size_t shift(right % 64); shift != 0) {
      Natural result{};
      Carryover carryover{};
      for (const std::uint64_t i : value.value) {
        auto overflow = carryover.overflow;
        carryover = right_shift(i, shift);
        result.value.emplace_back(carryover.result | overflow);
      }
      return Number(std::move(result));
    }
    return Number(std::move(value));
  }
  auto operator>>(const Natural &left, const Negative &right) -> Number {
    return std::visit([&left](const auto &value) { return left << value; },
                      right.value);
  }
  auto operator>>(const Natural & /*left*/, const Rational & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Natural & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Natural & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Negative &left, std::uint64_t right) -> Number {
    return std::visit([right](const auto &value) { return -(value >> right); },
                      left.value);
  }
  auto operator>>(const Negative &left, Base right) -> Number {
    return left >> right.value;
  }
  auto operator>>(const Negative &left, const Natural &right) -> Number {
    return std::visit([&right](const auto &value) { return -(value >> right); },
                      left.value);
  }
  auto operator>>(const Negative &left, const Negative &right) -> Number {
    return std::visit([](const auto &l, const auto &r) { return -(l << r); },
                      left.value, right.value);
  }
  auto operator>>(const Negative & /*left*/, const Rational & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Negative & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Negative & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Rational & /*left*/, std::uint64_t /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Rational & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Rational & /*left*/, const Natural & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Rational & /*left*/, const Negative & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Rational & /*left*/, const Rational & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Rational & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Rational & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Imag & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Imag & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Imag & /*left*/, const Natural & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Imag & /*left*/, const Negative & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Imag & /*left*/, const Rational & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Imag & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Imag & /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Complex & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Complex & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Complex & /*left*/, const Natural & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Complex & /*left*/, const Negative & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Complex & /*left*/, const Rational & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator>>(const Complex & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator>>(const Complex & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
} // namespace chimera::library::object::number

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
