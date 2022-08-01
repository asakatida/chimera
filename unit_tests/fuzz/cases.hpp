#pragma once

#include <cstddef>
#include <cstdint>

namespace chimera::library {
  auto fuzz_file_eval(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_parse_number(const std::uint8_t *data, std::size_t size) -> int;
} // namespace chimera::library
