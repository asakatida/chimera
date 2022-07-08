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

#pragma once

#include <limits>
#include <numeric>
#include <variant>

#include <gsl/gsl>
#include <tao/operators.hpp>

#include "object/number/base.hpp"
#include "object/number/complex.hpp"
#include "object/number/imag.hpp"
#include "object/number/natural.hpp"
#include "object/number/negative.hpp"
#include "object/number/rational.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

namespace chimera::library::object::number {
  using NumberValue =
      std::variant<Base, Natural, Negative, Rational, Imag, Complex>;
  class Number : tao::operators::commutative_bitwise<Number>,
                 tao::operators::equivalent<Number>,
                 tao::operators::field<Number>,
                 tao::operators::modable<Number>,
                 tao::operators::ordered_commutative_ring<Number>,
                 tao::operators::shiftable<Number>,
                 tao::operators::unit_steppable<Number> {
  public:
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    Number(std::uint64_t i);
    template <typename... Args>
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    Number(Args &&...args) : value(std::forward<Args>(args)...) {}
    void swap(Number &&other) noexcept;
    template <typename T>
    explicit operator T() const noexcept {
      return std::visit(Construct<T>{}, value);
    }
    [[nodiscard]] auto complex() const noexcept -> Number;
    [[nodiscard]] auto floor_div(const Number &right) const noexcept
        -> Number;
    [[nodiscard]] auto floor_div(const std::uint64_t right) const noexcept
        -> Number;
    [[nodiscard]] auto gcd(const Number &right) const noexcept -> Number;
    [[nodiscard]] auto gcd(const std::uint64_t right) const noexcept
        -> Number;
    [[nodiscard]] auto is_complex() const noexcept -> bool;
    [[nodiscard]] auto is_int() const noexcept -> bool;
    [[nodiscard]] auto pow(const Number &right) const noexcept -> Number;
    [[nodiscard]] auto pow(const std::uint64_t right) const noexcept
        -> Number;
    auto operator-() const noexcept -> Number;
    auto operator-=(const Number &right) noexcept -> Number;
    auto operator-=(const std::uint64_t right) noexcept -> Number;
    auto operator*=(const Number &right) noexcept -> Number;
    auto operator*=(const std::uint64_t right) noexcept -> Number;
    auto operator/=(const Number &right) noexcept -> Number;
    auto operator/=(const std::uint64_t right) noexcept -> Number;
    auto operator&=(const Number &right) noexcept -> Number;
    auto operator&=(const std::uint64_t right) noexcept -> Number;
    auto operator%=(const Number &right) noexcept -> Number;
    auto operator%=(const std::uint64_t right) noexcept -> Number;
    auto operator^=(const Number &right) noexcept -> Number;
    auto operator^=(const std::uint64_t right) noexcept -> Number;
    auto operator+() const noexcept -> Number;
    auto operator+=(const Number &right) noexcept -> Number;
    auto operator+=(const std::uint64_t right) noexcept -> Number;
    auto operator<(const Number &right) const noexcept -> bool;
    auto operator<(const std::uint64_t right) const noexcept -> bool;
    auto operator<<=(const Number &right) noexcept -> Number;
    auto operator<<=(const std::uint64_t right) noexcept -> Number;
    // auto operator==(const Number &right) const noexcept -> bool;
    auto operator==(const std::uint64_t right) const noexcept -> bool;
    auto operator>>=(const Number &right) noexcept -> Number;
    auto operator>>=(const std::uint64_t right) noexcept -> Number;
    auto operator|=(const Number &right) noexcept -> Number;
    auto operator|=(const std::uint64_t right) noexcept -> Number;
    auto operator~() const noexcept -> Number;
    template <typename OStream>
    auto debug(OStream &os) const noexcept -> OStream &;
    template <typename OStream>
    auto repr(OStream &os) const noexcept -> OStream &;

  private:
    template <typename Visitor>
    auto visit(Visitor &&visitor) const noexcept -> Number {
      return std::visit(std::forward<Visitor>(visitor), value);
    }
    template <typename Visitor>
    auto visit(const Number &right, Visitor &&visitor) const noexcept
        -> Number {
      return std::visit(std::forward<Visitor>(visitor), value, right.value);
    }
    NumberValue value;
  };
} // namespace chimera::library::object::number

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
