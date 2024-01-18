//! wrapper for Input*
//! provides global parse state tracking

#pragma once

#include <exception>                   // for throw_with_nested
#include <tao/pegtl/istream_input.hpp> // for istream_input

#include <cstdint>
#include <iosfwd>
#include <stack>
#include <variant>

namespace chimera::library::grammar {
  using InputBase = tao::pegtl::istream_input<>;
  struct Input : InputBase {
    Input(std::istream &input, const char *source);
    [[nodiscard]] auto dedent() -> bool;
    [[nodiscard]] auto indent() -> bool;
    [[nodiscard]] auto is_newline() const -> bool;

  private:
    [[nodiscard]] auto is_dedent() -> bool;
    [[nodiscard]] auto validate() -> bool;
    char indentType = '\0';
    std::stack<std::uintmax_t> indentStack{};
  };
} // namespace chimera::library::grammar
