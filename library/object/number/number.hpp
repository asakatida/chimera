#pragma once

#include <gsl/gsl>
#include <tao/operators.hpp>

#include <cstdint>
#include <limits>
#include <numeric>
#include <variant>
#include <vector>

// NOLINTBEGIN(readability-redundant-declaration)
using PythonNumber = uint64_t;

extern "C" {
auto r_repr(uint8_t *buffer, size_t capacity, PythonNumber value) -> int32_t;
auto r_repr_len(PythonNumber value) -> size_t;
} // extern "C"
// NOLINTEND(readability-redundant-declaration)

namespace chimera::library::object::number {
  class Number : tao::operators::commutative_bitwise<Number>,
                 tao::operators::modable<Number>,
                 tao::operators::ordered_field<Number>,
                 tao::operators::shiftable<Number>,
                 tao::operators::unit_steppable<Number> {
  public:
    Number();
    explicit Number(std::uint64_t number);
    Number(const Number &other);
    Number(Number &&other) noexcept;
    auto operator=(const Number &other) -> Number &;
    auto operator=(Number &&other) noexcept -> Number &;
    ~Number();
    void swap(Number &&other) noexcept;
    explicit operator int64_t() const noexcept;
    explicit operator uint64_t() const noexcept;
    explicit operator double() const noexcept;
    [[nodiscard]] auto operator+() const -> Number;
    [[nodiscard]] auto operator-() const -> Number;
    auto operator+=(const Number &right) -> Number &;
    auto operator-=(const Number &right) -> Number &;
    auto operator*=(const Number &right) -> Number &;
    auto operator/=(const Number &right) -> Number &;
    auto operator%=(const Number &right) -> Number &;
    [[nodiscard]] auto operator~() const -> Number;
    auto operator&=(const Number &right) -> Number &;
    auto operator|=(const Number &right) -> Number &;
    auto operator^=(const Number &right) -> Number &;
    auto operator<<=(const Number &right) -> Number &;
    auto operator>>=(const Number &right) -> Number &;
    [[nodiscard]] auto operator==(const Number &right) const -> bool;
    [[nodiscard]] auto operator<(const Number &right) const -> bool;
    [[nodiscard]] auto floor_div(const Number &right) const -> Number;
    [[nodiscard]] auto gcd(const Number &right) const -> Number;
    [[nodiscard]] auto pow(const Number &right) const -> Number;
    [[nodiscard]] auto pow(const Number &exp,
                           const Number &mod) const -> Number;
    [[nodiscard]] auto is_complex() const -> bool;
    [[nodiscard]] auto is_int() const -> bool;
    [[nodiscard]] auto is_nan() const -> bool;
    [[nodiscard]] auto imag() const -> Number;
    template <typename OStream>
    [[nodiscard]] auto debug(OStream &ostream) const -> OStream & {
      return repr(ostream);
    }
    template <typename OStream>
    auto repr(OStream &ostream) const -> OStream & {
      auto size = r_repr_len(ref);
      auto buffer = std::vector<std::uint8_t>(size);
      if (r_repr(buffer.data(), size, ref) != 0) {
        throw std::runtime_error("Failed to represent number");
      }
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      return ostream << reinterpret_cast<char *>(buffer.data());
    }

  private:
    Number(PythonNumber ref, bool /*unused*/) noexcept;
    PythonNumber ref;
  };
} // namespace chimera::library::object::number

template <typename OStream>
auto operator<<(OStream &ostream,
                const chimera::library::object::number::Number &number)
    -> OStream & {
  return number.debug(ostream);
}
