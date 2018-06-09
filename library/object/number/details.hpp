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

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <numeric>
#include <variant>
#include <vector>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        struct Base {
          std::uint64_t value;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) {
              return value != 0u;
            }
            if (value >= std::uint64_t(std::numeric_limits<T>::max())) {
              return std::numeric_limits<T>::max();
            }
            return T(value);
          }
        };
        struct Natural {
          std::vector<std::uint64_t> value;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) {
              return true;
            }
            if (value.size() > 2) {
              return std::numeric_limits<T>::max();
            }
            auto a = (__uint128_t(value[1]) << 64u) | value[0];
            if (a >= __uint128_t(std::numeric_limits<T>::max())) {
              return std::numeric_limits<T>::max();
            }
            return T(a);
          }
        };
        using PositiveValue = std::variant<Base, Natural>;
        struct Positive {
          PositiveValue value;

          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Positive(Base base);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Positive(Natural natural);

          Positive(const Positive &other) = default;
          Positive(Positive &&other) noexcept = default;

          ~Positive() noexcept = default;

          Positive &operator=(const Positive &other) = default;
          Positive &operator=(Positive &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) {
              return std::visit([](const auto &v) { return T(v); }, value);
            }
            return std::visit([](const auto &v) { return T(v); }, value);
          }
        };
        struct Negative {
          PositiveValue value;

          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Negative(Base base);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Negative(Natural natural);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Negative(Positive positive);

          Negative(const Negative &other) = default;
          Negative(Negative &&other) noexcept = default;

          ~Negative() noexcept = default;

          Negative &operator=(const Negative &other) = default;
          Negative &operator=(Negative &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) {
              return std::visit([](const auto &v) { return T(v); }, value);
            }
            return std::visit([](const auto &v) { return -T(v); }, value);
          }
        };
        using IntegerValue = std::variant<Base, Natural, Negative>;
        struct Integer {
          IntegerValue value;

          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Integer(Base base);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Integer(Natural natural);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Integer(Positive positive);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Integer(Negative negative);

          Integer(const Integer &other) = default;
          Integer(Integer &&other) noexcept = default;

          ~Integer() noexcept = default;

          Integer &operator=(const Integer &other) = default;
          Integer &operator=(Integer &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            return std::visit([](const auto &v) { return T(v); }, value);
          }
        };
        struct Rational {
          IntegerValue numerator;
          IntegerValue denominator;

          template <typename Numerator, typename Denominator>
          Rational(Numerator _numerator, Denominator _denominator) : numerator(Integer(_numerator).value), denominator(Integer(_denominator).value) {}

          Rational(const Rational &other) = default;
          Rational(Rational &&other) noexcept = default;

          ~Rational() noexcept = default;

          Rational &operator=(const Rational &other) = default;
          Rational &operator=(Rational &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) {
              return true;
            }
            return std::visit(
                [](const auto &n, const auto &d) { return T(n) / T(d); },
                numerator, denominator);
          }
        };
        using RealValue = std::variant<Base, Natural, Negative, Rational>;
        struct Real {
          RealValue value;

          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Real(Base base);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Real(Natural natural);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Real(Positive positive);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Real(Negative negative);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Real(Integer integer);
          // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
          Real(Rational rational);

          Real(const Real &other) = default;
          Real(Real &&other) noexcept = default;

          ~Real() noexcept = default;

          Real &operator=(const Real &other) = default;
          Real &operator=(Real &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            return std::visit([](const auto &v) { return T(v); }, value);
          }
        };
        using NumberValue = std::variant<Base, Natural, Negative, Rational>;
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
