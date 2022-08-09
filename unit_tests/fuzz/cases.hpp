#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>

#include "grammar/grammar.hpp"

namespace chimera::library {
  using Input = grammar::Input<tao::pegtl::istream_input<>>;
  auto fuzz_file_eval(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_file_eval(std::istream &&in) -> int;
  auto fuzz_parse_ellipsis(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_parse_ellipsis(Input &&in) -> int;
  auto fuzz_parse_false(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_parse_false(Input &&in) -> int;
  auto fuzz_parse_keywords(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_parse_keywords(Input &&in) -> int;
  auto fuzz_parse_name(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_parse_name(Input &&in) -> int;
  auto fuzz_parse_none(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_parse_none(Input &&in) -> int;
  auto fuzz_parse_number(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_parse_number(Input &&in) -> int;
  auto fuzz_parse_string(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_parse_string(Input &&in) -> int;
  auto fuzz_parse_true(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_parse_true(Input &&in) -> int;
} // namespace chimera::library
