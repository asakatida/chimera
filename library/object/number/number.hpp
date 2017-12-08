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
#include <variant>
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

        struct Number {
          NumberValue value;

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
        };

        inline Number number(std::uint64_t value) {
          return Number{Base{value}};
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
