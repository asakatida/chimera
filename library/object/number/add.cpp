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

#include "object/number/add.hpp"

#include <variant>
#include <vector>

#include <gsl/assert>
#include <tao/operators.hpp>

#include "object/number/mult.hpp"
#include "object/number/overflow.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera::library::object::number {
  struct Plus {
    template <typename Left>
    auto operator()(const Left &left, const Rational &right) -> Number {
      return std::visit(
          [&left](const auto &rN, const auto &rD) {
            return (rN + left * rD) / rD;
          },
          right.numerator, right.denominator);
    }
    template <typename Right>
    auto operator()(const Rational &left, const Right &right) -> Number {
      return std::visit(
          [&right](const auto &lN, const auto &lD) {
            return (lN + right * lD) / lD;
          },
          left.numerator, left.denominator);
    }
  };
  auto operator+(std::uint64_t left, Base right) -> Number {
    return right + left;
  }
  auto operator+(std::uint64_t left, const Natural &right) -> Number {
    return right + left;
  }
  auto operator+(std::uint64_t left, const Negative &right) -> Number {
    return right + left;
  }
  auto operator+(std::uint64_t left, const Rational &right) -> Number {
    return right + left;
  }
  auto operator+(std::uint64_t /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(std::uint64_t /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(Base left, std::uint64_t right) -> Number {
    auto value = sum(left.value, right);
    if (value.overflow == 0) {
      return Number(Base{value.result});
    }
    return Number(Natural{{value.result, value.overflow}});
  }
  auto operator+(Base left, Base right) -> Number { return left + right.value; }
  auto operator+(Base left, const Natural &right) -> Number {
    return right + left;
  }
  auto operator+(Base left, const Negative &right) -> Number {
    return right + left;
  }
  auto operator+(Base left, const Rational &right) -> Number {
    return right + left;
  }
  auto operator+(Base /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(Base /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Natural &left, std::uint64_t right) -> Number {
    if (right == 0) {
      return Number(Natural(left));
    }
    Natural value;
    value.value.reserve(left.value.size());
    Carryover carryover{0, right};
    for (const std::uint64_t i : left.value) {
      carryover = sum(i, carryover.overflow);
      value.value.emplace_back(carryover.result);
    }
    if (carryover.overflow != 0) {
      value.value.emplace_back(carryover.overflow);
    }
    return Number(std::move(value));
  }
  auto operator+(const Natural &left, Base right) -> Number {
    return left + right.value;
  }
  auto operator+(const Natural &left, const Natural &right) -> Number {
    Natural value;
    value.value.reserve(std::max(left.value.size(), right.value.size()));
    auto it1 = left.value.begin();
    auto end1 = left.value.end();
    auto it2 = right.value.begin();
    auto end2 = right.value.end();
    Carryover carryover{};
    for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
      auto next = sum(*it1, *it2);
      carryover = sum(carryover.result, next.result);
      value.value.emplace_back(carryover.result);
      carryover = sum(carryover.overflow, next.overflow);
      Ensures(carryover.overflow == 0);
    }
    if (it1 == end1) {
      it1 = it2;
      end1 = end2;
    }
    carryover.overflow = carryover.result;
    for (; it1 != end1; ++it1) {
      carryover = sum(carryover.overflow, *it1);
      value.value.emplace_back(carryover.result);
    }
    if (carryover.overflow != 0) {
      value.value.emplace_back(carryover.overflow);
    }
    return Number(std::move(value));
  }
  auto operator+(const Natural &left, const Negative &right) -> Number {
    return right + left;
  }
  auto operator+(const Natural &left, const Rational &right) -> Number {
    return right + left;
  }
  auto operator+(const Natural & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Natural & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator+(const Negative &left, std::uint64_t right) -> Number {
    return std::visit([right](const auto &value) { return right - value; },
                      left.value);
  }
  auto operator+(const Negative &left, Base right) -> Number {
    return left + right.value;
  }
  auto operator+(const Negative &left, const Natural &right) -> Number {
    return std::visit([&right](const auto &value) { return right - value; },
                      left.value);
  }
  auto operator+(const Negative &left, const Negative &right) -> Number {
    return std::visit([](const auto &l, const auto &r) { return -(l + r); },
                      left.value, right.value);
  }
  auto operator+(const Negative &left, const Rational &right) -> Number {
    return right + left;
  }
  auto operator+(const Negative & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Negative & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator+(const Rational &left, std::uint64_t right) -> Number {
    return std::visit(
        [right](const auto &lN, const auto &lD) {
          return (lN + right * lD) / lD;
        },
        left.numerator, left.denominator);
  }
  auto operator+(const Rational &left, Base right) -> Number {
    return left + right.value;
  }
  auto operator+(const Rational &left, const Natural &right) -> Number {
    return std::visit(
        [&right](const auto &lN, const auto &lD) {
          return (lN + right * lD) / lD;
        },
        left.numerator, left.denominator);
  }
  auto operator+(const Rational &left, const Negative &right) -> Number {
    return std::visit(
        [&right](const auto &lN, const auto &lD) {
          return (lN + right * lD) / lD;
        },
        left.numerator, left.denominator);
  }
  auto operator+(const Rational &left, const Rational &right) -> Number {
    return std::visit(
        [](const auto &lN, const auto &lD, const auto &rN, const auto &rD) {
          return (lN * rD + rN * lD) / (lD * rD);
        },
        left.numerator, left.denominator, right.numerator, right.denominator);
  }
  auto operator+(const Rational & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Rational & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator+(const Imag & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Imag & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Imag & /*left*/, const Natural & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Imag & /*left*/, const Negative & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Imag & /*left*/, const Rational & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Imag & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Imag & /*left*/, const Complex & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Complex & /*left*/, std::uint64_t /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Complex & /*left*/, Base /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Complex & /*left*/, const Natural & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator+(const Complex & /*left*/, const Negative & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator+(const Complex & /*left*/, const Rational & /*right*/)
      -> Number {
    Expects(false);
  }
  auto operator+(const Complex & /*left*/, const Imag & /*right*/) -> Number {
    Expects(false);
  }
  auto operator+(const Complex & /*left*/, const Complex & /*right*/)
      -> Number {
    Expects(false);
  }

} // namespace chimera::library::object::number
