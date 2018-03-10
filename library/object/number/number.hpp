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
#include <variant> // for variant
#include <vector>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        struct Base {
          std::uint64_t value;
        };
        struct Natural {
          std::vector<std::uint64_t> value;
        };
        using IntegerValue = std::variant<Base, Natural>;
        struct Integer {
          IntegerValue value;
        };
        using RationalValue = std::variant<Base, Natural, Integer>;
        struct Rational {
          RationalValue numerator;
          RationalValue denominator;
        };
        using NumberValue = std::variant<Base, Natural, Integer, Rational>;

        class Number {
        public:
          Number() noexcept = default;
          explicit Number(std::uint64_t i);
          explicit Number(Base base);
          explicit Number(Natural natural);
          explicit Number(Integer integer);
          explicit Number(Rational rational);

          Number(const Number &other) = default;
          Number(Number &&other) noexcept = default;

          ~Number() noexcept = default;

          Number &operator=(const Number &other) = default;
          Number &operator=(Number &&other) noexcept = default;

          Number operator+() const;
          Number operator-() const;
          Number operator+(const Number &right) const;
          Number &operator+=(const Number &right) const;
          Number operator-(const Number &right) const;
          Number &operator-=(const Number &right) const;
          Number operator*(const Number &right) const;
          Number &operator*=(const Number &right) const;
          Number operator/(const Number &right) const;
          Number &operator/=(const Number &right) const;
          Number operator%(const Number &right) const;
          Number &operator%=(const Number &right) const;
          Number operator~() const;
          Number operator&(const Number &right) const;
          Number &operator&=(const Number &right) const;
          Number operator|(const Number &right) const;
          Number &operator|=(const Number &right) const;
          Number operator^(const Number &right) const;
          Number &operator^=(const Number &right) const;
          Number operator<<(const Number &right) const;
          Number &operator<<=(const Number &right) const;
          Number operator>>(const Number &right) const;
          Number &operator>>=(const Number &right) const;
          bool operator==(const Number &right) const;
          bool operator!=(const Number &right) const;
          bool operator<(const Number &right) const;
          bool operator>(const Number &right) const;
          bool operator<=(const Number &right) const;
          bool operator>=(const Number &right) const;

          Number floor_div(const Number &right) const;

          Number pow(const Number &right) const;
          Number pow(const Number &y, const Number &z) const;

          bool is_int() const;

          bool is_complex() const;

          Number complex() const;

          template <typename Visitor>
          auto visit(Visitor &&visitor) const {
            return std::visit(std::forward<Visitor>(visitor), value);
          }

          template <typename Visitor>
          auto visit(const Number &right, Visitor &&visitor) const {
            return std::visit(std::forward<Visitor>(visitor), value,
                              right.value);
          }

        private:
          NumberValue value;
        };
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
