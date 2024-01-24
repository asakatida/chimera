#include "object/number/number.hpp"

#include "number-rust.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

namespace chimera::library::object::number {
  Number::Number() : ref(r_create_number(0)) {}
  Number::Number(std::uint64_t number) : ref(r_create_number(number)) {}
  Number::Number(PythonNumber ref, bool /*unused*/) noexcept : ref(ref) {}
  Number::Number(const Number &other) : ref(r_copy_number(other.ref)) {}
  Number::Number(Number &&other) noexcept : ref(0) { swap(std::move(other)); }
  auto Number::operator=(const Number &other) -> Number & {
    if (this != &other) {
      auto ptr = r_copy_number(other.ref);
      using std::swap;
      swap(ptr, ref);
      r_delete_number(ptr);
    }
    return *this;
  }
  auto Number::operator=(Number &&other) noexcept -> Number & {
    if (this != &other) {
      swap(std::move(other));
    }
    return *this;
  }
  Number::~Number() { r_delete_number(ref); }
  void Number::swap(Number &&other) noexcept {
    using std::swap;
    swap(ref, other.ref);
  }
  Number::operator int64_t() const noexcept { return r_cast_int(ref); }
  Number::operator uint64_t() const noexcept { return r_cast_unsigned(ref); }
  Number::operator double() const noexcept { return r_cast_float(ref); }
#define NUM_OP_MONO(op, name)                                                  \
  auto Number::operator op() const -> Number { return {name(ref), false}; }
#define NUM_OP(op, name)                                                       \
  auto Number::operator op(const Number & right) -> Number & {                 \
    auto ptr = name(ref, right.ref);                                           \
    using std::swap;                                                           \
    swap(ptr, ref);                                                            \
    r_delete_number(ptr);                                                      \
    return *this;                                                              \
  }
#define NUM_OP_NAMED(op, name)                                                 \
  [[nodiscard]] auto Number::op(const Number &right) const -> Number {         \
    return {name(ref, right.ref), false};                                      \
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
  [[nodiscard]] auto Number::operator==(const Number &right) const -> bool {
    return r_eq(ref, right.ref);
  }
  [[nodiscard]] auto Number::operator<(const Number &right) const -> bool {
    return r_lt(ref, right.ref);
  }
  [[nodiscard]] auto Number::floor_div(const Number &right) const -> Number {
    return {r_floor_div(ref, right.ref), false};
  }
  NUM_OP_NAMED(gcd, r_gcd)
  NUM_OP_NAMED(pow, r_pow)
  [[nodiscard]] auto Number::pow(const Number &exp, const Number &mod) const
      -> Number {
    return {r_mod_pow(ref, exp.ref, mod.ref), false};
  }
  [[nodiscard]] auto Number::is_complex() const -> bool {
    return r_is_complex(ref);
  }
  [[nodiscard]] auto Number::is_int() const -> bool { return r_is_int(ref); }
  [[nodiscard]] auto Number::is_nan() const -> bool { return r_is_nan(ref); }
  [[nodiscard]] auto Number::imag() const -> Number {
    return {r_imag(ref), false};
  }
} // namespace chimera::library::object::number

// NOLINTEND(cppcoreguidelines-macro-usage)
