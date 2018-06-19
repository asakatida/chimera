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

#pragma once

#include <cstdint>
#include <functional>
#include <numeric>
#include <variant>
#include <vector>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        struct ReducedValue {};

        struct Base {
          std::uint64_t value;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) { // NOLINT
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
            if constexpr (std::is_same_v<T, bool>) { // NOLINT
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

          explicit Positive(Base base);
          explicit Positive(Natural natural);
          Positive(ReducedValue /*unused*/, Base base);
          Positive(ReducedValue /*unused*/, Natural natural);

          Positive(const Positive &other) = default;
          Positive(Positive &&other) noexcept = default;

          ~Positive() noexcept = default;

          Positive &operator=(const Positive &other) = default;
          Positive &operator=(Positive &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) { // NOLINT
              return std::visit([](const auto &v) { return T(v); }, value);
            }
            return std::visit([](const auto &v) { return T(v); }, value);
          }
        };
        struct Negative {
          PositiveValue value;

          explicit Negative(Base base);
          explicit Negative(Natural natural);
          explicit Negative(Positive positive);
          Negative(ReducedValue /*unused*/, Base base);
          Negative(ReducedValue /*unused*/, Natural natural);
          Negative(ReducedValue /*unused*/, Positive positive);

          Negative(const Negative &other) = default;
          Negative(Negative &&other) noexcept = default;

          ~Negative() noexcept = default;

          Negative &operator=(const Negative &other) = default;
          Negative &operator=(Negative &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) { // NOLINT
              return std::visit([](const auto &v) { return T(v); }, value);
            }
            return std::visit([](const auto &v) { return -T(v); }, value);
          }
        };
        using IntegerValue = std::variant<Base, Natural, Negative>;
        struct Integer {
          IntegerValue value;

          explicit Integer(Base base);
          explicit Integer(Natural natural);
          explicit Integer(Positive positive);
          explicit Integer(Negative negative);
          Integer(ReducedValue /*unused*/, Base base);
          Integer(ReducedValue /*unused*/, Natural natural);
          Integer(ReducedValue /*unused*/, Positive positive);
          Integer(ReducedValue /*unused*/, Negative negative);

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
          Rational(Numerator num, Denominator den)
              : numerator(Integer(num).value), denominator(Integer(den).value) {
          }
          template <typename Numerator, typename Denominator>
          Rational(ReducedValue /*unused*/, Numerator num, Denominator den)
              : numerator(num), denominator(den) {}

          Rational(const Rational &other) = default;
          Rational(Rational &&other) noexcept = default;

          ~Rational() noexcept = default;

          Rational &operator=(const Rational &other) = default;
          Rational &operator=(Rational &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) { // NOLINT
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

          explicit Real(Base base);
          explicit Real(Natural natural);
          explicit Real(Positive positive);
          explicit Real(Negative negative);
          explicit Real(Integer integer);
          explicit Real(Rational rational);
          template <typename Numerator, typename Denominator>
          Real(Numerator num, Denominator den)
              : Real(Rational(num, den)) {}
          Real(ReducedValue /*unused*/, Base base);
          Real(ReducedValue /*unused*/, Natural natural);
          Real(ReducedValue /*unused*/, Positive positive);
          Real(ReducedValue /*unused*/, Negative negative);
          Real(ReducedValue /*unused*/, Integer integer);
          Real(ReducedValue /*unused*/, Rational rational);
          template <typename Numerator, typename Denominator>
          Real(ReducedValue /*unused*/, Numerator num, Denominator den)
              : Real(ReducedValue{}, Rational(ReducedValue{}, num, den)) {}

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
