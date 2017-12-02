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

#include "object/number/number.hpp"

#include <tuple> // for tuple
#include <type_traits>

#include "object/number/add.hpp"
#include "object/number/and.hpp"
#include "object/number/compare.hpp"
#include "object/number/div.hpp"
#include "object/number/invert.hpp"
#include "object/number/left_shift.hpp"
#include "object/number/less.hpp"
#include "object/number/mod.hpp"
#include "object/number/mult.hpp"
#include "object/number/negative.hpp"
#include "object/number/or.hpp"
#include "object/number/overflow.hpp"
#include "object/number/positive.hpp"
#include "object/number/right_shift.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"
#include "object/number/xor.hpp"

namespace chimera {
  namespace library {
    namespace object {

      namespace number {
        Number Number::operator+() const {
          return std::visit([](auto a) { return +a; }, value);
        }
        Number Number::operator-() const {
          return std::visit([](auto a) { return -a; }, value);
        }
        Number Number::operator+(const Number &right) const {
          return std::visit([](auto a, auto b) { return a + b; }, value,
                            right.value);
        }
        Number Number::operator-(const Number &right) const {
          return std::visit([](auto a, auto b) { return a - b; }, value,
                            right.value);
        }
        Number Number::operator*(const Number &right) const {
          return std::visit([](auto a, auto b) { return a * b; }, value,
                            right.value);
        }
        Number Number::operator/(const Number &right) const {
          return std::visit([](auto a, auto b) { return a / b; }, value,
                            right.value);
        }
        Number Number::operator%(const Number &right) const {
          return std::visit([](auto a, auto b) { return a % b; }, value,
                            right.value);
        }
        Number Number::operator~() const {
          return std::visit([](auto a) { return ~a; }, value);
        }
        Number Number::operator&(const Number &right) const {
          return std::visit([](auto a, auto b) { return a & b; }, value,
                            right.value);
        }
        Number Number::operator|(const Number &right) const {
          return std::visit([](auto a, auto b) { return a | b; }, value,
                            right.value);
        }
        Number Number::operator^(const Number &right) const {
          return std::visit([](auto a, auto b) { return a ^ b; }, value,
                            right.value);
        }
        Number Number::operator<<(const Number &right) const {
          return std::visit([](auto a, auto b) { return a << b; }, value,
                            right.value);
        }
        Number Number::operator>>(const Number &right) const {
          return std::visit([](auto a, auto b) { return a >> b; }, value,
                            right.value);
        }
        bool Number::operator==(const Number &right) const {
          return value.index() == right.value.index() &&
                 std::visit(
                     [](auto a, auto b) { return !((a < b) || (b < a)); },
                     value, right.value);
        }
        bool Number::operator!=(const Number &right) const {
          return value.index() != right.value.index() ||
                 std::visit([](auto a, auto b) { return (a < b) || (b < a); },
                            value, right.value);
        }
        bool Number::operator<(const Number &right) const {
          return std::visit([](auto a, auto b) { return a < b; }, value,
                            right.value);
        }
        bool Number::operator>(const Number &right) const {
          return std::visit([](auto a, auto b) { return b < a; }, value,
                            right.value);
        }
        bool Number::operator<=(const Number &right) const {
          return std::visit([](auto a, auto b) { return !(b < a); }, value,
                            right.value);
        }
        bool Number::operator>=(const Number &right) const {
          return std::visit([](auto a, auto b) { return !(a < b); }, value,
                            right.value);
        }

        Number Number::pow(const Number &right) const { return right; }
        Number Number::pow(const Number &y, const Number & /*z*/) const {
          return y;
        }

        bool Number::is_int() const {
          return std::holds_alternative<Base>(value) ||
                 std::holds_alternative<Natural>(value) ||
                 std::holds_alternative<Integer>(value);
        }

        bool Number::is_complex() const { return false; }

        Number Number::complex() const { return *this; }
      } // namespace number
    }   // namespace object
  }     // namespace library
};      // namespace chimera
