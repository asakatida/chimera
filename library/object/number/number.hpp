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
#include <limits>
#include <numeric>
#include <variant>
#include <vector>

#include <gsl/gsl>
#include <tao/operators.hpp>

using PythonNumber = uint64_t;

extern "C" {
int32_t r_repr(uint8_t *s, PythonNumber value);
size_t r_repr_len(PythonNumber value);
} // extern "C"

namespace chimera::library::object::number {
  class Number : tao::operators::commutative_bitwise<Number>,
                 tao::operators::modable<Number>,
                 tao::operators::ordered_field<Number>,
                 tao::operators::shiftable<Number>,
                 tao::operators::unit_steppable<Number> {
  public:
    Number();
    explicit Number(std::uint64_t i);
    Number(const Number &other);
    Number(Number &&other) noexcept;
    Number &operator=(const Number &other);
    Number &operator=(Number &&other) noexcept;
    ~Number();
    void swap(Number &&other) noexcept;
    explicit operator int64_t() const noexcept;
    explicit operator uint64_t() const noexcept;
    explicit operator double() const noexcept;
    auto operator+() const -> Number;
    auto operator-() const -> Number;
    auto operator+=(const Number &right) -> Number &;
    auto operator-=(const Number &right) -> Number &;
    auto operator*=(const Number &right) -> Number &;
    auto operator/=(const Number &right) -> Number &;
    auto operator%=(const Number &right) -> Number &;
    auto operator~() const -> Number;
    auto operator&=(const Number &right) -> Number &;
    auto operator|=(const Number &right) -> Number &;
    auto operator^=(const Number &right) -> Number &;
    auto operator<<=(const Number &right) -> Number &;
    auto operator>>=(const Number &right) -> Number &;
    auto operator==(const Number &right) const -> bool;
    auto operator<(const Number &right) const -> bool;
    [[nodiscard]] auto floor_div(const Number &right) const -> Number;
    [[nodiscard]] auto gcd(const Number &right) const -> Number;
    [[nodiscard]] auto pow(const Number &right) const -> Number;
    [[nodiscard]] auto pow(const Number &y, const Number &z) const -> Number;
    [[nodiscard]] auto is_complex() const -> bool;
    [[nodiscard]] auto is_int() const -> bool;
    [[nodiscard]] auto is_nan() const -> bool;
    [[nodiscard]] auto imag() const -> Number;
    template <typename OStream>
    auto debug(OStream &os) const -> OStream & {
      return repr(os);
    }
    template <typename OStream>
    auto repr(OStream &os) const -> OStream & {
      std::string buffer(r_repr_len(ref), '\0');
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      if (r_repr(reinterpret_cast<std::uint8_t *>(buffer.data()), ref) != 0) {
        throw std::runtime_error("Failed to represent number");
      }
      return os << buffer;
    }

  private:
    Number(PythonNumber ref, bool _unused) noexcept;
    PythonNumber ref;
  };
} // namespace chimera::library::object::number

template <typename OStream>
auto operator<<(OStream &os,
                const chimera::library::object::number::Number &number)
    -> OStream & {
  return number.debug(os);
}
