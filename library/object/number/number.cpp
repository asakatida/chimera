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

#include "number-rust.hpp"

namespace chimera::library::object::number {
  Number::Number() : value(r_create_number(0)) {}
  Number::Number(std::uint64_t i) : value(r_create_number(i)) {}
  Number::Number(PythonNumber &value) : value(&value) {}
  Number::Number(const Number &other) : value(r_copy_number(other.value)) {}
  Number::Number(Number &&other) noexcept { swap(std::move(other)); }
  Number &Number::operator=(const Number &other) {
    auto ptr = r_copy_number(other.value);
    using std::swap;
    swap(ptr, value);
    if (ptr != nullptr) {
      r_delete_number(ptr);
    }
    return *this;
  }
  Number &Number::operator=(Number &&other) noexcept {
    swap(std::move(other));
    return *this;
  }
  Number::~Number() {
    if (value != nullptr) {
      auto ptr = std::move(value);
      r_delete_number(ptr);
    }
  }
  void Number::swap(Number &&other) noexcept {
    using std::swap;
    swap(value, other.value);
  }
  Number::operator int64_t() const noexcept { return r_cast_int(value); }
  Number::operator uint64_t() const noexcept { return r_cast_unsigned(value); }
  Number::operator double() const noexcept { return r_cast_float(value); }
#define NUM_OP_MONO(op, name)                                                  \
  auto Number::operator op() const->Number { return Number(*name(value)); }
#define NUM_OP(op, name)                                                       \
  auto Number::operator op(const Number &right)->Number & {                    \
    auto ptr = name(value, right.value);                                       \
    using std::swap;                                                           \
    swap(ptr, value);                                                          \
    if (ptr != nullptr) {                                                      \
      r_delete_number(ptr);                                                    \
    }                                                                          \
    return *this;                                                              \
  }
#define NUM_OP_NAMED(op, name)                                                 \
  auto Number::op(const Number &right) const->Number {                         \
    return Number(*name(value, right.value));                                  \
  }
  NUM_OP_MONO(-, r_neg)
  NUM_OP_MONO(+, r_abs)
  NUM_OP_MONO(~, r_bit_not)
  NUM_OP(-=, r_sub)
  NUM_OP(*=, r_mul)
  NUM_OP(/=, r_div)
  NUM_OP(&=, r_bit_and)
  NUM_OP(%=, r_modu)
  NUM_OP(^=, r_bit_xor)
  NUM_OP(+=, r_add)
  NUM_OP(<<=, r_bit_lshift)
  NUM_OP(>>=, r_bit_rshift)
  NUM_OP(|=, r_bit_or)
  auto Number::operator==(const Number &right) const -> bool {
    return r_eq(value, right.value);
  }
  auto Number::operator<(const Number &right) const -> bool {
    return r_lt(value, right.value);
  }
  auto Number::floor_div(const Number &right) const -> Number { return right; }
  NUM_OP_NAMED(gcd, r_gcd)
  NUM_OP_NAMED(pow, r_pow)
  auto Number::pow(const Number &y, const Number & /*z*/) const -> Number {
    return y;
  }
  auto Number::is_complex() const -> bool { return r_is_complex(value); }
  auto Number::is_int() const -> bool { return r_is_int(value); }
  auto Number::is_nan() const -> bool { return r_is_nan(value); }
  auto Number::imag() const -> Number { return Number(*r_imag(value)); }
} // namespace chimera::library::object::number
