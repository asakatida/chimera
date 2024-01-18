//! template options for parse rules.

#pragma once

#include <type_traits>

namespace chimera::library::grammar::flags {
  enum {
    ASYNC_FLOW,
    DISCARD,
    IMPLICIT,
    IMPORT_ALL,
    LOOP_FLOW,
    SCOPE_FLOW,
  };
  using Flag = std::uint8_t;
  template <auto... Flags>
  constexpr static Flag list = ((1U << Flags) | ... | 0U);
  template <Flag Options, auto... Flags>
  constexpr static Flag mask = Options & ((1U << Flags) | ...);
  template <Flag Options, auto... Flags>
  constexpr static bool get = mask<Options, Flags...> != 0;
  template <Flag Options, auto... Flags>
  constexpr static Flag set = Options;
  // constexpr static Flag set = ((1U << Flags) | ... | Options);
  template <Flag Options, auto... Flags>
  constexpr static Flag unSet = ((~(1U << Flags)) & ... & Options);
} // namespace chimera::library::grammar::flags
